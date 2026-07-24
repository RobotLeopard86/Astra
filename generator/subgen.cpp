#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

#include "nlohmann/json.hpp"

//Recursively unwrap std::unique_ptr and std::shared_ptr
std::string unwrapPointer(std::string type) {
	std::regex ptrRegex(R"(std::(?:unique|shared)_ptr\s*<\s*(.*)\s*>)");
	std::smatch match;
	while(std::regex_search(type, match, ptrRegex)) {
		type = match[1].str();
	}
	return type;
}

//Recursively transform types to their substitutes if reflectable
std::string transformType(const std::string& type, const std::vector<std::string>& reflectableTypes) {
	//Handle templates (like std::vector<T>)
	if(type.find('<') != std::string::npos && type.find('>') != std::string::npos) {
		size_t start = type.find('<');
		size_t end = type.find_last_of('>');
		std::string prefix = type.substr(0, start + 1);
		std::string suffix = type.substr(end + 1);
		std::string inner = type.substr(start + 1, end - start - 1);

		//This is a simple implementation for single-argument templates.
		//For multi-argument, a proper parser would be needed.
		return prefix + transformType(inner, reflectableTypes) + suffix;
	}

	//Check if the type is reflectable
	for(const auto& rt : reflectableTypes) {
		if(type == rt) {
			return "astra::SerializedSubstitute<" + rt + ">";
		}
	}
	return type;
}

void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results) {
	std::vector<std::string> reflectableTypes;
	for(auto const& [name, data] : results) {
		if(data.contains("kind") && data["kind"] == 0 && name.find("astra::SerializedSubstitute") == std::string::npos) {
			reflectableTypes.push_back(name);
		}
	}

	for(const auto& typeName : reflectableTypes) {
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

			//1. Recursive pointer unwrapping
			std::string unwrapped = unwrapPointer(originalType);

			//2. Apply substitution to the unwrapped type (and nested templates)
			std::string substituted = transformType(unwrapped, reflectableTypes);

			field["type"] = substituted;
			field["original_type"] = originalType;//Store for re-wrapping in deserialize()

			//Markers for template logic
			field["is_substitute_type"] = (substituted.find("astra::SerializedSubstitute") != std::string::npos);
			field["is_pointer"] = (originalType != unwrapped);
			field["is_vector"] = (originalType.find("std::vector") != std::string::npos);
		}

		subJson["fields"] = std::move(fields);
		subJson["methods"] = nlohmann::json::array();

		results[subName] = std::move(subJson);
	}
}