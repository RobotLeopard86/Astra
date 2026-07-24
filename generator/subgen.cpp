#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <algorithm>

#include "nlohmann/json.hpp"

struct UnwrapResult {
	std::string rawType;
	std::vector<std::string> wrappers;//"unique", "shared", or "raw"
};

//Recursively unwrap std::unique_ptr, std::shared_ptr, and raw pointers T*
UnwrapResult unwrapAndTrack(std::string type) {
	UnwrapResult res;

	//1. Handle Smart Pointers
	std::regex ptrRegex(R"(std::(unique|shared)_ptr\s*<\s*(.*)\s*>)");
	std::smatch match;
	while(std::regex_search(type, match, ptrRegex)) {
		res.wrappers.push_back(match[1].str());
		type = match[2].str();
	}

	//2. Handle Raw Pointers (T*)
	while(!type.empty() && type.back() == '*') {
		res.wrappers.push_back("raw");
		type.pop_back();
		//Strip trailing spaces or qualifiers
		while(!type.empty() && (std::isspace(type.back()) ||
								   (type.size() >= 5 && type.substr(type.size() - 5) == "const") ||
								   (type.size() >= 9 && type.substr(type.size() - 9) == "volatile"))) {
			if(type.size() >= 5 && type.substr(type.size() - 5) == "const")
				type.erase(type.size() - 5);
			else if(type.size() >= 9 && type.substr(type.size() - 9) == "volatile")
				type.erase(type.size() - 9);
			else
				type.pop_back();
		}
	}

	res.rawType = type;
	return res;
}

//Build the re-wrap expression for deserialize(): handles std::make_... and raw new
std::string generateReWrap(const UnwrapResult& unwrap, const std::string& memberName) {
	if(unwrap.wrappers.empty()) return memberName + ".deserialize()";

	std::string currentExpr = memberName + ".deserialize()";
	std::string currentType = unwrap.rawType;

	for(auto it = unwrap.wrappers.rbegin(); it != unwrap.wrappers.rend(); ++it) {
		std::string wrapper = *it;
		if(wrapper == "unique") {
			currentExpr = "std::make_unique<" + currentType + ">(" + currentExpr + ")";
		} else if(wrapper == "shared") {
			currentExpr = "std::make_shared<" + currentType + ">(" + currentExpr + ")";
		} else {//raw
			currentExpr = "new " + currentType + "(" + currentExpr + ")";
		}
		currentType = "std::" + wrapper + (wrapper == "raw" ? "" : "_ptr") + "<" + currentType + ">";
	}
	return currentExpr;
}

std::string generateDeref(const UnwrapResult& unwrap, const std::string& memberName) {
	if(unwrap.wrappers.empty()) return "original." + memberName;
	std::string expr = "original." + memberName;
	for(size_t i = 0; i < unwrap.wrappers.size(); ++i) expr = "*" + expr;
	return expr;
}

//Recursively transform types to their substitutes, handling template arguments (e.g. vector<vector<T>>)
std::string transformType(const std::string& type, const std::vector<std::string>& reflectableTypes) {
	if(type.find('<') == std::string::npos) {
		for(const auto& rt : reflectableTypes) {
			if(type == rt) return "astra::SerializedSubstitute<" + rt + ">";
		}
		return type;
	}

	size_t start = type.find('<');
	size_t end = type.find_last_of('>');
	std::string prefix = type.substr(0, start + 1);
	std::string inner = type.substr(start + 1, end - start - 1);
	std::string suffix = type.substr(end + 1);

	//Handle multi-argument templates (like std::map<K, V>)
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
			} else {
				current += c;
			}
		}
		args.push_back(current);
		if(args.size() == 2) {
			return prefix + transformType(args[0], reflectableTypes) + ", " + transformType(args[1], reflectableTypes) + ">" + suffix;
		}
	}

	return prefix + transformType(inner, reflectableTypes) + ">" + suffix;
}

void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results) {
	std::vector<std::string> reflectableTypes;
	std::vector<std::string> reflectableClasses;
	for(auto const& [name, data] : results) {
		if(data.contains("kind") && name.find("astra::SerializedSubstitute") == std::string::npos) {
			reflectableTypes.push_back(name);
			if(data["kind"] == 0) reflectableClasses.push_back(name);
		}
	}

	for(const auto& typeName : reflectableClasses) {
		std::string subName = "astra::SerializedSubstitute<" + typeName + ">";
		if(results.count(subName)) continue;

		const auto& originalData = results[typeName];
		nlohmann::json subJson;
		subJson["kind"] = 0;
		subJson["name"] = subName;
		subJson["is_substitute"] = true;
		subJson["original_type"] = typeName;
		subJson["origin"] = originalData["origin"];
		subJson["namespace"] = "astra";

		auto fields = originalData["fields"];
		for(auto& field : fields) {
			std::string originalType = field["type"];

			//1. Unwrap pointers
			UnwrapResult unwrap = unwrapAndTrack(originalType);
			std::string rawType = unwrap.rawType;

			//2. Qualify nested types post-unwrapping
			if(rawType.find("::") == std::string::npos && rawType.find("std::") == std::string::npos) {
				std::string qualified = typeName + "::" + rawType;
				for(const auto& rt : reflectableTypes) {
					if(rt == qualified) {
						rawType = qualified;
						break;
					}
				}
			}

			//3. Determine substitute type
			std::string substituted = transformType(rawType, reflectableClasses);

			//Handle container conversion to vector for non-random access
			if(unwrap.wrappers.empty() && originalType.find("std::") == 0 && originalType.find('<') != std::string::npos) {
				std::string prefix = originalType.substr(0, originalType.find('<') + 1);
				bool isRandom = (prefix == "std::vector<" || prefix == "std::array<" || prefix == "std::deque<" ||
								 prefix == "std::map<" || prefix == "std::unordered_map<");
				if(!isRandom) {
					field["type"] = "std::vector<" + substituted + ">";
				} else {
					field["type"] = transformType(originalType, reflectableClasses);
				}
			} else {
				field["type"] = substituted;
			}

			field["original_type"] = originalType;
			field["rewrap_expr"] = generateReWrap(unwrap, field["safe_name"]);
			field["deref_expr"] = generateDeref(unwrap, field["safe_name"]);
			field["is_substitute_type"] = (field["type"].get<std::string>().find("astra::SerializedSubstitute") != std::string::npos);
			field["is_pointer"] = !unwrap.wrappers.empty();

			//Categorize container for template deserialization
			std::string kind = "none";
			if(originalType.find("std::vector<") == 0 || originalType.find("std::deque<") == 0 ||
				originalType.find("std::list<") == 0 || originalType.find("std::stack<") == 0 ||
				originalType.find("std::queue<") == 0)
				kind = "vector";
			else if(originalType.find("std::set<") == 0)
				kind = "set";
			else if(originalType.find("std::map<") == 0 || originalType.find("std::unordered_map<") == 0)
				kind = "map";
			else if(originalType.find("std::array<") == 0)
				kind = "array";
			field["container_kind"] = kind;
		}

		subJson["fields"] = std::move(fields);
		subJson["methods"] = nlohmann::json::array();
		results[subName] = std::move(subJson);
	}
}