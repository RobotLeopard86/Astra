#include <format>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <set>

#include "nlohmann/json.hpp"

#include "to_filename.hpp"

struct Converter {
	std::string original;	//Original type name
	std::string transformed;//Output type name after conversion
	enum class Operation {
		Direct,		 //Copy identical objects
		Substitution,//Original object <-> astra::SerializedSubstitute
		UniquePtr,	 //Original wrapped in unique_ptr
		SharedPtr,	 //Original wrapped in shared_ptr
		RawPtr,		 //Original wrapped in raw pointer
		STLConvert,	 //Original is an STL type mapped to a different one
		List,		 //Original is a list/array
		Map			 //Original is a map
	} op;
	enum class ListInsertionType {
		Push,
		PushBack,
		Insert,
		Assign
	} listInsertion;
};

struct ConverterChainNode {
	ConverterChainNode* prev;
	Converter* self;
	ConverterChainNode* next1;
	ConverterChainNode* next2;
};

std::string tryQualifyType(const std::string& source, const std::string& holder, const std::vector<std::string>& reflectableTypes) {
	std::string work = source;
	if(source.find("::") == std::string::npos && !source.starts_with("std::")) {
		std::string qualified = holder + "::" + source;
		for(const auto& rt : reflectableTypes) {
			if(rt.compare(qualified) == 0) {
				work = qualified;
				break;
			}
		}
		if(source.compare(qualified) != 0) {
			std::size_t lastScope = holder.find_last_of("::");
			if(lastScope != std::string::npos) return tryQualifyType(source, holder.substr(0, lastScope), reflectableTypes);
		}
	}
	return work;
}

std::string stripQualifiers(std::string type) {
	while(!type.empty() && (std::isspace(type.back()) || (type.size() >= 5 && type.substr(type.size() - 5).compare("const") == 0) || (type.size() >= 9 && type.substr(type.size() - 9).compare("volatile") == 0))) {
		if(type.size() >= 5 && type.substr(type.size() - 5).compare("const") == 0)
			type.erase(type.size() - 5);
		else if(type.size() >= 9 && type.substr(type.size() - 9).compare("volatile") == 0)
			type.erase(type.size() - 9);
		else
			type.pop_back();
	}
	return type;
}

#define continue_if_next(result)                  \
	if(auto r = result; node->next1)              \
		return getSerializedType(r, node->next1); \
	else                                          \
		return r;
std::string getSerializedType(const std::string& source, ConverterChainNode* node) {
	switch(node->self->op) {
		case Converter::Operation::Direct:
			return source;
		case Converter::Operation::Substitution:
			continue_if_next(std::format("astra::SerializedSubstitute<{}>", source));
		case Converter::Operation::UniquePtr:
			continue_if_next(source.substr(strlen("std::unique_ptr") + 1, source.size() - strlen("std::unique_ptr") - 2));
		case Converter::Operation::SharedPtr:
			continue_if_next(source.substr(strlen("std::shared_ptr") + 1, source.size() - strlen("std::shared_ptr") - 2));
		case Converter::Operation::RawPtr:
			continue_if_next(source.substr(0, source.size() - 1));
		case Converter::Operation::STLConvert:
			continue_if_next(node->self->transformed);
		case Converter::Operation::List: {
			std::string ltype = source.substr(0, source.find_first_of('<'));
			std::string inner = source.substr(source.find_first_of('<') + 1, source.find_last_of('>') - (source.find_first_of('<') + 1));
			if(!node->next1) throw std::runtime_error("Invalid list node!");
			return std::format("{}<{}>", ltype, getSerializedType(inner, node->next1));
		}
		case Converter::Operation::Map: {
			std::string mtype = source.starts_with("std::unordered") ? "std::unordered_map" : "std::map";
			unsigned int brackets = 0;
			unsigned int i;
			for(i = mtype.size() + 1; i < source.size(); ++i) {
				if(source[i] == '<')
					++brackets;
				else if(source[i] == '>')
					--brackets;
				else if(source[i] == ',' && brackets == 0)
					break;
			}
			std::string a1 = source.substr(mtype.size() + 1, i - (mtype.size() + 1));
			std::string a2 = source.substr(i + 2, source.find_last_of('>') - (i + 2));
			if(!node->next1 || !node->next2) throw std::runtime_error("Invalid map node!");
			return std::format("{}<{}, {}>", mtype, getSerializedType(a1, node->next1), getSerializedType(a2, node->next2));
		}
		default: return source;
	}
}
#undef continue_if_next

ConverterChainNode* createConverterChain(const std::string& source, std::set<std::string>& substitutes, std::deque<Converter>& converters, const std::vector<std::string>& reflectableTypes, const std::vector<std::string>& reflectableClasses, ConverterChainNode* prev) {
	//Check for smart pointers
	if(source.starts_with("std::unique_ptr<") || source.starts_with("std::shared_ptr<")) {
		//Gather template arguments
		bool isUnique = source.starts_with("std::unique_ptr<");
		size_t start = source.find('<');
		size_t end = source.find_last_of('>');
		std::string inner = source.substr(start + 1, end - start - 1);

		//Set up converter
		Converter c;
		c.original = source;
		c.transformed = inner;
		c.op = isUnique ? Converter::Operation::UniquePtr : Converter::Operation::SharedPtr;
		converters.push_back(c);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		node->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, node);
		return node;
	}

	//Check for raw pointers
	if(!source.empty() && source.back() == '*' && source.compare("const char*") != 0) {
		//Find underlying type
		std::string inner = source;
		inner.pop_back();
		inner = stripQualifiers(inner);

		//Set up converter
		Converter c;
		c.original = source;
		c.transformed = inner;
		c.op = Converter::Operation::RawPtr;
		converters.push_back(c);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		node->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, node);
		return node;
	}

	//Check for STL containers
	if(source.starts_with("std::") && source.find('<') != std::string::npos) {
		//Gather template arguments
		size_t start = source.find('<');
		size_t end = source.find_last_of('>');
		std::string prefix = source.substr(0, start + 1);
		std::string inner = source.substr(start + 1, end - start - 1);

		//Is this a map?
		if(prefix.compare("std::map<") == 0 || prefix.compare("std::unordered_map<") == 0) {
			//Split key and value
			std::vector<std::string> args;
			int depth = 0;
			std::string current;
			for(char c : inner) {
				if(c == '<') depth++;
				if(c == '>') depth--;
				if(c == ',' && depth == 0) {
					args.push_back(current);
					current.clear();
				} else
					current += c;
			}
			args.push_back(current.substr(1));

			//Check for 2 args (otherwise we're bugged)
			if(args.size() == 2) {
				//Set up converter
				Converter c;
				c.original = source;
				c.op = Converter::Operation::Map;
				converters.push_back(c);

				//Make and return node
				ConverterChainNode* node = new ConverterChainNode();
				node->self = &converters[converters.size() - 1];
				node->prev = prev;
				node->next1 = createConverterChain(args[0], substitutes, converters, reflectableTypes, reflectableClasses, node);
				node->next2 = createConverterChain(args[1], substitutes, converters, reflectableTypes, reflectableClasses, node);
				converters[converters.size() - 1].transformed = prefix + getSerializedType(args[0], node->next1) + ", " + getSerializedType(args[1], node->next2) + ">";
				return node;
			}
		}

		//Lists and arrays otherwise (random access is for narrowing STL containers to easy-to-serialize types)
		bool isRandomAccess = (prefix.compare("std::vector<") == 0 || prefix.compare("std::array<") == 0);
		if(!isRandomAccess) {
			//Set up converter to go from some STL type to a vector
			Converter conv;
			conv.original = source;
			conv.op = Converter::Operation::STLConvert;
			conv.transformed = "std::vector<" + inner + ">";
			conv.listInsertion = (prefix.compare("std::set<") == 0) ? ((prefix.compare("std::queue<") == 0 || prefix.compare("std::stack<") == 0) ? Converter::ListInsertionType::Push : Converter::ListInsertionType::PushBack) : Converter::ListInsertionType::Insert;
			converters.push_back(conv);

			//Make node
			ConverterChainNode* node = new ConverterChainNode();
			node->prev = prev;
			node->self = &converters[converters.size() - 1];

			//Set up converter for unpacking the vector into elements
			Converter listC;
			listC.original = "std::vector<" + inner + ">";
			listC.transformed = inner;
			listC.op = Converter::Operation::List;
			listC.listInsertion = Converter::ListInsertionType::PushBack;
			converters.push_back(listC);

			//Make the unpacking node
			ConverterChainNode* listNode = new ConverterChainNode();
			listNode->prev = node;
			listNode->self = &converters[converters.size() - 1];
			listNode->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, listNode);

			//Attach nodes and return
			node->next1 = listNode;
			return node;
		} else {
			//We're a vector or array, just do the unpacking
			Converter c;
			c.original = source;
			c.transformed = inner;
			c.op = Converter::Operation::List;
			c.listInsertion = (prefix.compare("std::array<") == 0) ? Converter::ListInsertionType::Assign : Converter::ListInsertionType::PushBack;
			converters.push_back(c);

			//Make and return node
			ConverterChainNode* node = new ConverterChainNode();
			node->prev = prev;
			node->self = &converters[converters.size() - 1];
			node->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, node);
			return node;
		}
	}

	//String substitution
	if(source.compare("std::string") == 0 || source.compare("std::string_view") == 0 || source.compare("const char*") == 0) {
		//Set up converter
		Converter c;
		c.original = source;
		c.op = Converter::Operation::STLConvert;
		c.transformed = "std::string";
		converters.push_back(c);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	//Replace STL integer aliases
	if(source.compare("std::byte") == 0) {
		//Set up converter
		Converter c;
		c.original = source;
		c.op = Converter::Operation::STLConvert;
		c.transformed = "unsigned char";
		converters.push_back(c);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}
	if(source.compare("std::size_t") == 0) {
		//Set up converter
		Converter c;
		c.original = source;
		c.op = Converter::Operation::STLConvert;
		c.transformed = "uint64_t";
		converters.push_back(c);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	//Check for reflectable type
	bool isReflectable = false;
	for(const auto& rt : reflectableTypes) {
		if(rt == source) {
			isReflectable = true;
			break;
		}
	}
	if(!isReflectable) {
		//Not reflectable, write type directly
		//This shouldn't happen often because if your class has non-reflectable types, you should make an explicit substitute
		Converter c;
		c.original = source;
		c.transformed = source;
		c.op = Converter::Operation::Direct;
		converters.push_back(c);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	//Check for class (only classes can have autogenerated substitutes)
	bool isClass = false;
	for(const auto& rc : reflectableClasses) {
		if(rc == source) {
			isClass = true;
			break;
		}
	}
	if(isClass) {
		//Set up converter
		Converter c;
		c.original = source;
		c.transformed = "astra::SerializedSubstitute<" + source + ">";
		c.op = Converter::Operation::Substitution;
		converters.push_back(c);

		//Log in substitutes list for header inclusion
		substitutes.insert(c.transformed);

		//Make and return node
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	//This should be for enums only; write directly
	Converter c;
	c.original = source;
	c.transformed = source;
	c.op = Converter::Operation::Direct;
	converters.push_back(c);

	//Make and return node
	ConverterChainNode* node = new ConverterChainNode();
	node->prev = prev;
	node->self = &converters[converters.size() - 1];
	return node;
}

void describeConverterChain(nlohmann::json& json, nlohmann::json& current, ConverterChainNode* node) {
	//Add current step
	nlohmann::json& step = current["steps"].emplace_back(nlohmann::json::object());
	step["original_type"] = node->self->original;
	step["serialized_type"] = node->self->transformed;
	switch(node->self->op) {
		case Converter::Operation::Direct:
			step["op"] = "direct";
			break;
		case Converter::Operation::Substitution:
			step["op"] = "sub";
			break;
		case Converter::Operation::UniquePtr:
			step["op"] = "unique";
			break;
		case Converter::Operation::SharedPtr:
			step["op"] = "shared";
			break;
		case Converter::Operation::RawPtr:
			step["op"] = "raw";
			break;
		case Converter::Operation::STLConvert:
			step["op"] = "stlcvt";
			step["mapped"] = node->self->transformed;
			switch(node->self->listInsertion) {
				case Converter::ListInsertionType::Push:
					step["insertion"] = "push";
					break;
				case Converter::ListInsertionType::PushBack:
					step["insertion"] = "push_back";
					break;
				case Converter::ListInsertionType::Insert:
					step["insertion"] = "insert";
					break;
				case Converter::ListInsertionType::Assign:
					step["insertion"] = "assign";
					break;
			}
			break;
		case Converter::Operation::List:
			step["op"] = "list";
			switch(node->self->listInsertion) {
				case Converter::ListInsertionType::Push:
					step["insertion"] = "push";
					break;
				case Converter::ListInsertionType::PushBack:
					step["insertion"] = "push_back";
					break;
				case Converter::ListInsertionType::Insert:
					step["insertion"] = "insert";
					break;
				case Converter::ListInsertionType::Assign:
					step["insertion"] = "assign";
					break;
			}
			step["item_cvt"] = current["fn_name"].get<std::string>() + "_item";
			current["serialized_type"] = node->self->transformed;
			{
				if(!node->next1) throw std::runtime_error("Invalid list node!");
				nlohmann::json& itemCvt = json.emplace_back(nlohmann::json::object());
				itemCvt["fn_name"] = step["item_cvt"];
				itemCvt["original_type"] = node->next1->self->original;
				itemCvt["steps"] = nlohmann::json::array();
				describeConverterChain(json, itemCvt, node->next1);
			}
			return;
		case Converter::Operation::Map:
			step["op"] = "map";
			step["key_cvt"] = current["fn_name"].get<std::string>() + "_key";
			step["val_cvt"] = current["fn_name"].get<std::string>() + "_val";
			current["serialized_type"] = node->self->transformed;
			{
				if(!node->next1) throw std::runtime_error("Invalid map node!");
				nlohmann::json& kcvt = json.emplace_back(nlohmann::json::object());
				kcvt["fn_name"] = step["key_cvt"];
				kcvt["original_type"] = node->next1->self->original;
				kcvt["steps"] = nlohmann::json::array();
				describeConverterChain(json, kcvt, node->next1);
			}
			{
				if(!node->next2) throw std::runtime_error("Invalid map node!");
				nlohmann::json& vcvt = json.emplace_back(nlohmann::json::object());
				vcvt["fn_name"] = step["val_cvt"];
				vcvt["original_type"] = node->next2->self->original;
				vcvt["steps"] = nlohmann::json::array();
				describeConverterChain(json, vcvt, node->next2);
			}
			return;
	}

	//Continue with next step
	if(node->next1)
		describeConverterChain(json, current, node->next1);
	else
		current["serialized_type"] = node->self->transformed;
}

void destroyConverterChain(ConverterChainNode* node) {
	//Destroy children recursively first
	if(node->next1) {
		destroyConverterChain(node->next1);
		node->next1 = nullptr;
	}
	if(node->next2) {
		destroyConverterChain(node->next2);
		node->next2 = nullptr;
	}
	delete node;
}

void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results) {
	//Separate into reflectable classes and enums + classes
	std::vector<std::string> reflectableTypes;
	std::vector<std::string> reflectableClasses;
	for(auto const& [name, data] : results) {
		if(data.contains("kind") && name.find("astra::SerializedSubstitute") == std::string::npos) {
			reflectableTypes.push_back(name);
			if(data["kind"] == 0) reflectableClasses.push_back(name);
		}
	}

	//Generate substitutes for classes
	for(const std::string& clazz : reflectableClasses) {
		//Check that we haven't seen this class before
		std::string subName = "astra::SerializedSubstitute<" + clazz + ">";
		if(results.count(subName)) continue;

		//Get original class info and set up substitute info
		const nlohmann::json& original = results[clazz];
		nlohmann::json& substitute = results[subName];
		substitute["kind"] = 0;
		substitute["name"] = subName;
		substitute["is_substitute"] = true;
		substitute["original_type"] = clazz;
		substitute["origin"] = original["origin"];
		substitute["namespace"] = "astra";
		substitute["methods"] = nlohmann::json::array();
		substitute["fields"] = nlohmann::json::array();
		substitute["converters"] = nlohmann::json::array();

		//Process fields
		std::set<std::string> substitutes;
		for(const nlohmann::json& field : original["fields"]) {
			//Get field type
			std::string originalType = tryQualifyType(field["type"], clazz, reflectableTypes);

			//Generate converter for field
			std::deque<Converter> converterObjs;
			ConverterChainNode* cvt = createConverterChain(originalType, substitutes, converterObjs, reflectableTypes, reflectableClasses, nullptr);

			//Set up field object
			nlohmann::json fieldDesc = nlohmann::json::object();
			fieldDesc["name"] = field["name"];
			fieldDesc["alias"] = field["name"];
			fieldDesc["acc"] = nlohmann::json::array({std::string("Public")});
			fieldDesc["type"] = getSerializedType(originalType, cvt);

			//Add converter information
			nlohmann::json& cvtJson = substitute["converters"].emplace_back(nlohmann::json::object());
			cvtJson["fn_name"] = field["name"];
			cvtJson["original_type"] = cvt->self->original;
			cvtJson["steps"] = nlohmann::json::array();
			describeConverterChain(substitute["converters"], cvtJson, cvt);

			//Add field to list
			substitute["fields"].push_back(fieldDesc);

			//Clean up converter chain
			destroyConverterChain(cvt);
		}

		//Generate headers list
		substitute["headers"] = nlohmann::json::array();
		for(const auto& sub : substitutes) {
			substitute["headers"].push_back(toFilename(sub));
		}
	}
}