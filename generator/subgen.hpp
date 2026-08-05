#pragma once

#include <unordered_map>
#include <string>

#include "nlohmann/json.hpp"

struct InheritedData {
	std::string basePath;
	std::unordered_map<std::string, nlohmann::json> results;
};

void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results, const std::vector<InheritedData>& inherited);