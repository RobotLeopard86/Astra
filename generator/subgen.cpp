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
	std::string original; //Original type name
	std::string stlMapped;//Replaced type for STLConvert operation type
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
	while(!type.empty() && (std::isspace(type.back()) || (type.size() >= 5 && type.substr(type.size() - 5) == "const") || (type.size() >= 9 && type.substr(type.size() - 9) == "volatile"))) {
		if(type.size() >= 5 && type.substr(type.size() - 5).compare("const") == 0)
			type.erase(type.size() - 5);
		else if(type.size() >= 9 && type.substr(type.size() - 9).compare("volatile") == 0)
			type.erase(type.size() - 9);
		else
			type.pop_back();
	}
	return type;
}

ConverterChainNode* createConverterChain(const std::string& source, std::set<std::string>& substitutes, std::deque<Converter>& converters, const std::vector<std::string>& reflectableTypes, const std::vector<std::string>& reflectableClasses, ConverterChainNode* prev) {
	//1. Smart Pointers
	if(source.starts_with("std::unique_ptr<") || source.starts_with("std::shared_ptr<")) {
		bool isUnique = source.starts_with("std::unique_ptr<");
		size_t start = source.find('<');
		size_t end = source.find_last_of('>');
		std::string inner = source.substr(start + 1, end - start - 1);

		Converter c;
		c.original = source;
		c.op = isUnique ? Converter::Operation::UniquePtr : Converter::Operation::SharedPtr;
		converters.push_back(c);

		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		node->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, node);
		return node;
	}

	//2. Raw Pointers
	if(!source.empty() && source.back() == '*') {
		std::string inner = source;
		inner.pop_back();
		inner = stripQualifiers(inner);

		Converter c;
		c.original = source;
		c.op = Converter::Operation::RawPtr;
		converters.push_back(c);

		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		node->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, node);
		return node;
	}

	//3. Containers
	if(source.starts_with("std::") && source.find('<') != std::string::npos) {
		size_t start = source.find('<');
		size_t end = source.find_last_of('>');
		std::string prefix = source.substr(0, start + 1);
		std::string inner = source.substr(start + 1, end - start - 1);

		//Maps
		if(prefix == "std::map<" || prefix == "std::unordered_map<") {
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
			if(args.size() == 2) {
				Converter c;
				c.original = source;
				c.op = Converter::Operation::Map;
				converters.push_back(c);
				ConverterChainNode* node = new ConverterChainNode();
				node->self = &converters[converters.size() - 1];
				node->prev = prev;
				node->next1 = createConverterChain(args[0], substitutes, converters, reflectableTypes, reflectableClasses, node);
				node->next2 = createConverterChain(args[1], substitutes, converters, reflectableTypes, reflectableClasses, node);
				return node;
			}
		}

		//Lists / Arrays
		bool isRandomAccess = (prefix == "std::vector<" || prefix == "std::array<");

		//If non-random access, we need an STLConvert step to std::vector first
		if(!isRandomAccess) {
			Converter conv;
			conv.original = source;
			conv.op = Converter::Operation::STLConvert;
			conv.stlMapped = "std::vector<" + inner + ">";
			converters.push_back(conv);

			ConverterChainNode* node = new ConverterChainNode();
			node->prev = prev;
			node->self = &converters[converters.size() - 1];

			//Now add the List operation for the vector
			Converter listC;
			listC.original = "std::vector<" + inner + ">";
			listC.op = Converter::Operation::List;
			listC.listInsertion = (prefix.compare("std::set<") == 0) ? ((prefix.compare("std::queue<") == 0 || prefix.compare("std::stack<") == 0) ? Converter::ListInsertionType::Push : Converter::ListInsertionType::PushBack) : Converter::ListInsertionType::Insert;
			converters.push_back(listC);

			ConverterChainNode* listNode = new ConverterChainNode();
			listNode->prev = node;
			listNode->self = &converters[converters.size() - 1];
			listNode->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, listNode);

			node->next1 = listNode;
			return node;
		} else {
			Converter c;
			c.original = source;
			c.op = Converter::Operation::List;
			c.listInsertion = (prefix == "std::array<") ? Converter::ListInsertionType::Assign : Converter::ListInsertionType::PushBack;
			converters.push_back(c);

			ConverterChainNode* node = new ConverterChainNode();
			node->prev = prev;
			node->self = &converters[converters.size() - 1];
			node->next1 = createConverterChain(inner, substitutes, converters, reflectableTypes, reflectableClasses, node);
			return node;
		}
	}

	//4. Base Type Flowchart
	if(source == "std::string" || source == "std::string_view" || source == "const char*") {
		Converter c;
		c.original = source;
		c.op = Converter::Operation::STLConvert;
		c.stlMapped = "std::string";
		converters.push_back(c);
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	if(source == "std::byte") {
		Converter c;
		c.original = source;
		c.op = Converter::Operation::STLConvert;
		c.stlMapped = "unsigned char";
		converters.push_back(c);
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	if(source == "std::size_t") {
		Converter c;
		c.original = source;
		c.op = Converter::Operation::STLConvert;
		c.stlMapped = "uint64_t";
		converters.push_back(c);
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	bool isReflectable = false;
	for(const auto& rt : reflectableTypes) {
		if(rt == source) {
			isReflectable = true;
			break;
		}
	}
	if(!isReflectable) {
		Converter c;
		c.original = source;
		c.op = Converter::Operation::Direct;
		converters.push_back(c);
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	bool isClass = false;
	for(const auto& rc : reflectableClasses) {
		if(rc == source) {
			isClass = true;
			break;
		}
	}

	if(isClass) {
		Converter c;
		c.original = source;
		c.op = Converter::Operation::Substitution;
		converters.push_back(c);
		substitutes.insert("astra::SerializedSubstitute<" + source + ">");
		ConverterChainNode* node = new ConverterChainNode();
		node->prev = prev;
		node->self = &converters[converters.size() - 1];
		return node;
	}

	Converter c;
	c.original = source;
	c.op = Converter::Operation::Direct;
	converters.push_back(c);
	ConverterChainNode* node = new ConverterChainNode();
	node->prev = prev;
	node->self = &converters[converters.size() - 1];
	return node;
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
			continue_if_next(node->self->stlMapped);
		case Converter::Operation::List:
			continue_if_next(source.substr(source.find_first_of('<') + 1, source.find_last_of('>') - (source.find_first_of('<') + 1)));
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

void describeConverterChain(nlohmann::json& json, ConverterChainNode* node) {
}

void destroyConverterChain(ConverterChainNode* node) {
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
			fieldDesc["acc"] = nlohmann::json::array({std::string("Public")});
			fieldDesc["type"] = getSerializedType(originalType, cvt);

			//Add converter information
			fieldDesc["cvt"] = nlohmann::json::array();
			nlohmann::json& cvtDesc = fieldDesc["cvt"];
			describeConverterChain(cvtDesc, cvt);

			//Add field to list
			substitute["fields"].push_back(fieldDesc);

			//Clean up converter chain
			destroyConverterChain(cvt);
		}

		//Generate headers list
		substitute["headers"] = nlohmann::json::array();
		for(const auto& sub : substitute) {
			substitute["headers"].push_back(toFilename(sub));
		}
	}
}