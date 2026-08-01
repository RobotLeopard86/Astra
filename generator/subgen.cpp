#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <set>

#include "nlohmann/json.hpp"

#include "to_filename.hpp"

struct UnwrapResult {
	std::string rawType;
	std::vector<std::string> wrappers;//"unique", "shared", or "raw"
};

struct Converter {
	std::string name;
	std::string originalType;
	std::string substitutedType;
	std::string kind;//"basic", "pointer", "vector", "set", "map_key", "map_val"
	std::string rewrap_expr;
	std::string deref_expr;
	bool is_substitute = false;
};

UnwrapResult unwrapAndTrack(std::string type) {
	UnwrapResult res;
	std::regex ptrRegex(R"(std::(unique|shared)_ptr\s*<\s*(.*)\s*>)");
	std::smatch match;
	while(std::regex_search(type, match, ptrRegex)) {
		res.wrappers.push_back(match[1].str());
		type = match[2].str();
	}
	while(!type.empty() && type.back() == '*') {
		res.wrappers.push_back("raw");
		type.pop_back();
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

std::string generateReWrap(const UnwrapResult& unwrap, const std::string& memberName, const std::string& typeName, const std::vector<std::string>& reflectableTypes) {
	if(unwrap.wrappers.empty()) return memberName + ".deserialize()";

	std::string currentExpr = memberName + ".deserialize()";
	std::string currentType = unwrap.rawType;

	if(currentType.find("::") == std::string::npos && currentType.find("std::") == std::string::npos) {
		std::string baseType = typeName;
	try_again_rw:
		std::string qualified = baseType + "::" + currentType;
		for(const auto& rt : reflectableTypes) {
			if(rt.compare(qualified) == 0) {
				currentType = qualified;
				break;
			}
		}
		if(currentType.compare(qualified) != 0) {
			std::size_t lastScope = baseType.find_last_of("::");
			if(lastScope != std::string::npos) {
				baseType = baseType.substr(0, lastScope);
				goto try_again_rw;
			}
		}
	}

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

std::string transformType(const std::string& type, const std::vector<std::string>& reflectableTypes, std::set<std::string>& headers) {
	if(type.find('<') == std::string::npos) {
		for(const auto& rt : reflectableTypes) {
			if(type.compare(rt) == 0) {
				headers.insert(rt);
				return "astra::SerializedSubstitute<" + rt + ">";
			}
		}
		return type;
	}
	size_t start = type.find('<'), end = type.find_last_of('>');
	std::string prefix = type.substr(0, start + 1), inner = type.substr(start + 1, end - start - 1), suffix = type.substr(end + 1);
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
		args.push_back(current);
		if(args.size() == 2) return prefix + transformType(args[0], reflectableTypes, headers) + ", " + transformType(args[1], reflectableTypes, headers) + ">" + suffix;
	}
	return prefix + transformType(inner, reflectableTypes, headers) + ">" + suffix;
}

void resolveConverters(const std::string& name, const std::string& type, const std::vector<std::string>& reflectableTypes, std::set<std::string>& headers, std::vector<Converter>& converters) {
	UnwrapResult unwrap = unwrapAndTrack(type);
	std::string raw = unwrap.rawType;

	//Qualify raw type if necessary
	//Note: this requires a reference to the current class name, omitted here for brevity in helper but integrated in main loop

	std::string subT = transformType(raw, reflectableTypes, headers);

	Converter conv;
	conv.name = name;
	conv.originalType = type;
	conv.substitutedType = subT;
	conv.is_substitute = (subT.find("astra::SerializedSubstitute") != std::string::npos);

	if(!unwrap.wrappers.empty()) {
		conv.kind = "pointer";
		conv.rewrap_expr = generateReWrap(unwrap, name, type, reflectableTypes);
		conv.deref_expr = generateDeref(unwrap, name);
	} else if(type.find("std::vector<") == 0 || type.find("std::deque<") == 0 || type.find("std::list<") == 0 || type.find("std::stack<") == 0 || type.find("std::queue<") == 0) {
		conv.kind = "vector";
		size_t start = type.find('<'), end = type.find_last_of('>');
		std::string inner = type.substr(start + 1, end - start - 1);
		resolveConverters(name + "_item", inner, reflectableTypes, headers, converters);
	} else if(type.find("std::set<") == 0) {
		conv.kind = "set";
		size_t start = type.find('<'), end = type.find_last_of('>');
		std::string inner = type.substr(start + 1, end - start - 1);
		resolveConverters(name + "_item", inner, reflectableTypes, headers, converters);
	} else if(type.find("std::map<") == 0 || type.find("std::unordered_map<") == 0) {
		conv.kind = "map";
		size_t start = type.find('<'), end = type.find_last_of('>');
		std::string inner = type.substr(start + 1, end - start - 1);
		std::vector<std::string> args;
		int depth = 0;
		std::string curr;
		for(char c : inner) {
			if(c == '<') depth++;
			if(c == '>') depth--;
			if(c == ',' && depth == 0) {
				args.push_back(curr);
				curr.clear();
			} else
				curr += c;
		}
		args.push_back(curr);
		if(args.size() == 2) {
			resolveConverters(name + "_key", args[0], reflectableTypes, headers, converters);
			resolveConverters(name + "_val", args[1], reflectableTypes, headers, converters);
		}
	} else if(type.find("std::array<") == 0) {
		conv.kind = "array";
		size_t start = type.find('<'), end = type.find_last_of('>');
		std::string inner = type.substr(start + 1, end - start - 1);
		resolveConverters(name + "_item", inner, reflectableTypes, headers, converters);
	} else {
		conv.kind = "basic";
	}
	converters.push_back(conv);
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

		std::vector<Converter> converters;
		std::set<std::string> headers;
		auto fields = originalData["fields"];
		for(auto& field : fields) {
			std::string originalType = field["type"];

			//Handle qualification before recursion
			if(originalType.find("::") == std::string::npos && originalType.find("std::") == std::string::npos) {
				std::string baseType = typeName;
			try_again_gs:
				std::string qualified = baseType + "::" + originalType;
				for(const auto& rt : reflectableTypes) {
					if(rt.compare(qualified) == 0) {
						originalType = qualified;
						break;
					}
				}
				if(originalType.compare(qualified) != 0) {
					std::size_t lastScope = baseType.find_last_of("::");
					if(lastScope != std::string::npos) {
						baseType = baseType.substr(0, lastScope);
						goto try_again_gs;
					}
				}
			}

			resolveConverters(field["name"], originalType, reflectableTypes, headers, converters);

			//The la-val field in the substitute is the flattened version
			UnwrapResult unwrap = unwrapAndTrack(originalType);
			field["type"] = transformType(unwrap.rawType, reflectableTypes, headers);

			//If it was a container, ensure it remains a container (transformed)
			if(unwrap.wrappers.empty() && originalType.find("std::") == 0 && originalType.find('<') != std::string::npos) {
				field["type"] = transformType(originalType, reflectableTypes, headers);
			}

			field["original_type"] = originalType;
			field["is_substitute_type"] = (field["type"].get<std::string>().find("astra::SerializedSubstitute") != std::string::npos);
		}

		nlohmann::json convJson = nlohmann::json::array();
		for(const auto& c : converters) {
			nlohmann::json j;
			j["name"] = c.name;
			j["originalType"] = c.originalType;
			j["substitutedType"] = c.substitutedType;
			j["kind"] = c.kind;
			j["rewrap_expr"] = c.rewrap_expr;
			j["deref_expr"] = c.deref_expr;
			j["is_substitute"] = c.is_substitute;
			convJson.push_back(j);
		}

		subJson["headers"] = nlohmann::json::array();
		for(const auto& rt : headers) {
			subJson["headers"].push_back(toFilename(rt));
		}
		subJson["converters"] = std::move(convJson);
		subJson["fields"] = std::move(fields);
		subJson["methods"] = nlohmann::json::array();
		results[subName] = std::move(subJson);
	}
}