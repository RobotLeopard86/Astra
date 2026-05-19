#pragma once

#include "clang/Frontend/CompilerInstance.h"

#include <string>
#include <unordered_map>

#include "nlohmann/json.hpp"

struct Context {
	std::string outputDir;
	std::unordered_map<std::string, nlohmann::json> result;
};
