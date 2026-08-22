#include <format>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <set>
#include <deque>

#include "nlohmann/json.hpp"

#include "to_filename.hpp"
#include "subgen.hpp"

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
};

struct ConverterChainNode {
	ConverterChainNode* prev;
	Converter* self;
	ConverterChainNode* next1;
	ConverterChainNode* next2;
};

std::vector<std::string_view> splitType(std::string_view typeName) {
	//Some setup
	std::vector<std::string_view> tokens;
	std::size_t begin = 0;
	auto emitText = [&](std::size_t end) {
		if(begin != end)
			tokens.emplace_back(typeName.substr(begin, end - begin));
	};

	//Scan the whole type name string
	for(std::size_t i = 0; i < typeName.size();) {
		switch(typeName[i]) {
			//Open template
			case '<':
				emitText(i);
				tokens.emplace_back(typeName.substr(i, 1));
				begin = ++i;
				break;

			//Template argument separation
			case ',': {
				//Whitespace control
				emitText(i);
				std::size_t end = i + 1;
				while(end < typeName.size() &&
					  std::isspace(static_cast<unsigned char>(typeName[end]))) {
					++end;
				}

				//Add token
				tokens.emplace_back(typeName.substr(i, end - i));
				begin = i = end;
				break;
			}

			//Close template
			case '>': {
				//Include all the closing angle brackets in one substring
				emitText(i);
				std::size_t end = i + 1;
				while(end < typeName.size() && typeName[end] == '>')
					++end;

				//Add token
				tokens.emplace_back(typeName.substr(i, end - i));
				begin = i = end;
				break;
			}

			default:
				++i;
				break;
		}
	}

	//Finish up and return
	emitText(typeName.size());
	return tokens;
}

std::string tryQualifyType(const std::string& source, const std::string& holder, const std::vector<std::string>& knownTypes) {
	//Gather template arguments
	std::vector<std::string_view> components = splitType(source);

	//If there are template arguments, then we need to process each one individually and then reconstitute the type
	if(components.size() > 1) {
		//We leave components that are just template bits alone and transform the rest
		auto transformed = components | std::views::transform([&holder, &knownTypes](std::string_view component) {
			std::string asStr(component);
			if(asStr.find_first_not_of("<> ,") == std::string::npos)
				return asStr;
			else
				return tryQualifyType(asStr, holder, knownTypes);
		}) | std::views::join |
						   std::views::common;

		//Return joined string
		return std::string(transformed.begin(), transformed.end());
	}

	//Go through all the reflectable types and compare them against an attempted qualification of the type name
	std::string work = source;
	if(source.find("::") == std::string::npos && !source.starts_with("std::")) {
		std::string qualified = holder + "::" + source;
		for(const auto& rt : knownTypes) {
			if(rt.compare(qualified) == 0) {
				work = qualified;
				break;
			}
		}

		//If we didn't find it, try going a scope up
		if(work.compare(qualified) != 0) {
			std::size_t lastScope = holder.find_last_of("::");
			if(lastScope != std::string::npos) return tryQualifyType(source, holder.substr(0, lastScope), knownTypes);
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

ConverterChainNode* createConverterChain(const std::string& source, std::set<std::string>& substitutes, std::deque<Converter>& converters, const std::vector<std::string>& substitutableTypes, ConverterChainNode* prev) {
	//Check for smart pointers
	if(source.starts_with("std::unique_ptr<") || source.starts_with("std::shared_ptr<")) {
		//Gather template arguments
		bool isUnique = source.starts_with("std::unique_ptr<");
		std::size_t start = source.find('<');
		std::size_t end = source.find_last_of('>');
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
		node->next1 = createConverterChain(inner, substitutes, converters, substitutableTypes, node);
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
		node->next1 = createConverterChain(inner, substitutes, converters, substitutableTypes, node);
		return node;
	}

	//Check for STL containers
	if(source.starts_with("std::") && source.find('<') != std::string::npos) {
		//Gather template arguments
		std::size_t start = source.find('<');
		std::size_t end = source.find_last_of('>');
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
				std::size_t cvtIndex = converters.size() - 1;

				//Make and return node
				ConverterChainNode* node = new ConverterChainNode();
				node->self = &converters[converters.size() - 1];
				node->prev = prev;
				node->next1 = createConverterChain(args[0], substitutes, converters, substitutableTypes, node);
				node->next2 = createConverterChain(args[1], substitutes, converters, substitutableTypes, node);
				converters[cvtIndex].transformed = prefix + getSerializedType(args[0], node->next1) + ", " + getSerializedType(args[1], node->next2) + ">";
				return node;
			}
		}

		//Lists and arrays otherwise (random access is for narrowing STL containers to easy-to-serialize types)
		bool isRandomAccess = (prefix.compare("std::vector<") == 0 || prefix.compare("std::deque<") == 0 || prefix.compare("std::array<") == 0);
		if(!isRandomAccess) {
			//Set up converter to go from some STL type to a vector
			Converter conv;
			conv.original = source;
			conv.op = Converter::Operation::STLConvert;
			conv.transformed = "std::vector<" + inner + ">";
			converters.push_back(conv);

			//Make node
			ConverterChainNode* node = new ConverterChainNode();
			node->prev = prev;
			node->self = &converters[converters.size() - 1];

			//Set up converter for unpacking the vector into elements
			Converter listC;
			listC.original = "std::vector<" + inner + ">";
			listC.op = Converter::Operation::List;
			converters.push_back(listC);
			std::size_t cvtIndex = converters.size() - 1;

			//Make the unpacking node
			ConverterChainNode* listNode = new ConverterChainNode();
			listNode->prev = node;
			listNode->self = &converters[converters.size() - 1];
			listNode->next1 = createConverterChain(inner, substitutes, converters, substitutableTypes, listNode);
			converters[cvtIndex].transformed = "std::vector<" + getSerializedType(inner, listNode->next1) + ">";

			//Attach nodes and return
			node->next1 = listNode;
			return node;
		} else {
			//We're a random-access type, just do the unpacking
			Converter c;
			c.original = source;
			if(inner.starts_with("std::array"))
				c.transformed = inner;
			else
				c.transformed = inner.substr(0, inner.find_last_of(','));
			c.op = Converter::Operation::List;
			converters.push_back(c);
			std::size_t cvtIndex = converters.size() - 1;

			//Make and return node
			ConverterChainNode* node = new ConverterChainNode();
			node->prev = prev;
			node->self = &converters[converters.size() - 1];
			node->next1 = createConverterChain(c.transformed, substitutes, converters, substitutableTypes, node);
			converters[cvtIndex].transformed = prefix + getSerializedType(inner, node->next1) + ">";
			return node;
		}
	}

	//String substitution
	if(source.compare("std::string_view") == 0 || source.compare("const char*") == 0) {
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

	//Direct std::string copy
	if(source.compare("std::string") == 0) {
		//Set up converter
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

	//Check for class (only classes can have autogenerated substitutes)
	bool isSubstitutable = false;
	for(const auto& st : substitutableTypes) {
		if(st.compare(source) == 0) {
			isSubstitutable = true;
			break;
		}
	}
	if(isSubstitutable) {
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
			step["container"] = !(node->self->transformed.compare("std::string") == 0 || node->self->transformed.compare("std::string_view") == 0 ||
								  node->self->transformed.compare("const char*") == 0 || node->self->transformed.compare("std::size_t") == 0 ||
								  node->self->transformed.compare("std::byte") == 0);
			break;
		case Converter::Operation::List:
			step["op"] = "list";
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

void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results, const std::vector<InheritedData>& inherited) {
	//Classify type names
	std::vector<std::string> generate;
	std::set<std::string> canSubstitute;
	std::set<std::string> allKnown;
	std::unordered_map<std::string, std::string> subOrigins;
	for(const auto& [name, data] : results) {
		if(!data.contains("kind")) continue;
		if(name.find("astra::SerializedSubstitute") == std::string::npos) {
			if(data["kind"] == 0) {
				generate.push_back(name);
				canSubstitute.insert(name);
				subOrigins[name] = "astra_generated/";
				subOrigins["astra::SerializedSubstitute<" + name + ">"] = "astra_generated/";
			}
		} else {
			std::string inner = name.substr(name.find_first_of('<') + 1, name.find_last_of('>') - name.find_first_of('<') - 1);
			canSubstitute.insert(inner);
			subOrigins[inner] = "astra_generated/";
			subOrigins[name] = "astra_generated/";
		}
		allKnown.insert(name);
	}
	for(const InheritedData& id : inherited) {
		for(const auto& [name, data] : id.results) {
			if(!data.contains("kind")) continue;
			if(name.find("astra::SerializedSubstitute") == std::string::npos) {
				if(data["kind"] == 0) {
					canSubstitute.insert(name);
					subOrigins[name] = id.basePath;
					subOrigins["astra::SerializedSubstitute<" + name + ">"] = id.basePath;
				}
			} else {
				std::string inner = name.substr(name.find_first_of('<') + 1, name.find_last_of('>') - name.find_first_of('<') - 1);
				canSubstitute.insert(inner);
				subOrigins[inner] = id.basePath;
				subOrigins[name] = id.basePath;
			}
			allKnown.insert(name);
		}
	}
	std::vector<std::string> substitutable(canSubstitute.begin(), canSubstitute.end());
	std::vector<std::string> knownTypes(allKnown.begin(), allKnown.end());

	//Generate substitutes for classes
	for(const std::string& clazz : generate) {
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
			std::string originalType = tryQualifyType(field["type"], clazz, knownTypes);

			//Generate converter for field
			std::deque<Converter> converterObjs;
			ConverterChainNode* cvt = createConverterChain(originalType, substitutes, converterObjs, substitutable, nullptr);

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
			if(auto serializedType = getSerializedType(cvt->self->original, cvt); cvt->self->original.compare(serializedType) != 0) {
				describeConverterChain(substitute["converters"], cvtJson, cvt);
			} else {
				//The chain results in no change, so we can "short-circuit" it and just directly pass the object through
				cvtJson["serialized_type"] = serializedType;
				nlohmann::json& step = cvtJson["steps"].emplace_back(nlohmann::json::object());
				step["original_type"] = cvt->self->original;
				step["serialized_type"] = serializedType;
				step["op"] = "direct";
			}

			//Add field to list
			substitute["fields"].push_back(fieldDesc);

			//Clean up converter chain
			destroyConverterChain(cvt);
		}

		//Generate headers list
		substitute["headers"] = nlohmann::json::array();
		for(const auto& sub : substitutes) {
			std::string filename = toFilename(sub);
			std::string dirLevelAdjust = "";
			for(char c : filename) {
				if(c == '/') dirLevelAdjust += "../";
			}
			substitute["headers"].push_back(dirLevelAdjust + subOrigins.at(sub) + filename + ".astra.hpp");
		}

		//Add data for required functions
		{
			nlohmann::json& deInternal = substitute["methods"].emplace_back(nlohmann::json::object());
			deInternal["name"] = "ASTRA__deserializeinternal";
			deInternal["alias"] = "ASTRA__deserializeinternal";
			deInternal["acc"] = nlohmann::json::array({std::string("Public")});
			deInternal["return"] = "void";
			deInternal["params"] = nlohmann::json::array();
			deInternal["params"].emplace_back("void*");
		}
		{
			nlohmann::json& dePublic = substitute["methods"].emplace_back(nlohmann::json::object());
			dePublic["name"] = "deserialize";
			dePublic["alias"] = "deserialize";
			dePublic["acc"] = nlohmann::json::array({std::string("Public")});
			dePublic["return"] = "void";
			dePublic["params"] = nlohmann::json::array();
			dePublic["params"].emplace_back(clazz + "*");
		}
		{
			nlohmann::json& enInternal = substitute["methods"].emplace_back(nlohmann::json::object());
			enInternal["name"] = "ASTRA__serializeinternal";
			enInternal["alias"] = "ASTRA__serializeinternal";
			enInternal["acc"] = nlohmann::json::array({std::string("Public")});
			enInternal["return"] = "void";
			enInternal["params"] = nlohmann::json::array();
			enInternal["params"].emplace_back("void*");
		}
		{
			nlohmann::json& enPublic = substitute["methods"].emplace_back(nlohmann::json::object());
			enPublic["name"] = "serialize";
			enPublic["alias"] = "serialize";
			enPublic["acc"] = nlohmann::json::array({std::string("Public")});
			enPublic["return"] = "void";
			enPublic["params"] = nlohmann::json::array();
			enPublic["params"].emplace_back(clazz + "*");
		}
	}
}