#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <string_view>

#include "nlohmann/json.hpp"

struct InheritedData {
	std::string basePath;
	std::unordered_map<std::string, nlohmann::json> results;
};

void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results, const std::vector<InheritedData>& inherited);

std::vector<std::string_view> splitType(std::string_view typeName);
std::string tryQualifyType(const std::string& source, const std::string& holder, const std::vector<std::string>& knownTypes);
