#pragma once

#include <string>
#include <vector>
#include <optional>
#include <set>

#include "context.hpp"

namespace clang {
	namespace tooling {
		class CompilationDatabase;
	}
}

class Parser {
  public:
	Parser(const std::string& compDBDir, const std::string& outputDir);
	~Parser();

	std::optional<std::unordered_map<std::string, nlohmann::json>> parse(const std::vector<std::string>& input);
	void findSysIncludes(const std::string& sample, const std::string& fallbackCompiler, bool fallbackMSVC);

  private:
	std::unique_ptr<clang::tooling::CompilationDatabase> compDB;
	std::set<std::string> sysincludes;
	Context ctx;
};
