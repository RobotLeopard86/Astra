#include "parser.hpp"

#include <stdexcept>

#include "action.hpp"

//clang
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CompilationDatabase.h"

namespace {
	std::unique_ptr<tooling::CompilationDatabase> loadCompdb(std::string_view compdbDir) {
		std::string err;

		auto source = StringRef(compdbDir.data(), compdbDir.size());
		auto compdb = tooling::CompilationDatabase::autoDetectFromDirectory(source, err);

		if(compdb == nullptr) {
			throw std::runtime_error("Cannot find compilation database, aborted");
		}
		return compdb;
	}
}

Parser::Parser(std::string_view compdbDir, std::string_view outputDir)
  : compDB(loadCompdb(compdbDir)) {
	ctx.outputDir = outputDir;
}

std::optional<std::unordered_map<std::string, nlohmann::json>> Parser::parse(const std::vector<std::string>& inputFiles) {
	tooling::ClangTool tool(*compDB, inputFiles);
	tool.appendArgumentsAdjuster([this](const tooling::CommandLineArguments& args, StringRef) {
		tooling::CommandLineArguments adjArgs;
		adjArgs.push_back(args[0]);
		adjArgs.push_back("-D_ASTRAGENERATE");
		std::set<std::string> used;
		for(const std::string& includeDir : sysincludes) {
			if(used.contains(includeDir)) continue;
#ifdef _WIN32
#define CPPSEARCH "c++\\"
#else
#define CPPSEARCH "c++/"
#endif
			if(includeDir.find(CPPSEARCH) != std::string::npos) {
				adjArgs.push_back(std::string("-isystem") + includeDir);
				used.insert(includeDir);
			}
#undef CPPSEARCH
		}
		if(auto it = std::find_if(sysincludes.cbegin(), sysincludes.cend(), [](const std::string& si) {
#ifdef _WIN32
			   return si.find("lib\\clang") != std::string::npos;
#else
			   return si.find("lib/clang") != std::string::npos;
#endif
		   });
			it != sysincludes.cend()) {
			adjArgs.push_back(std::string("-isystem") + *it);
			used.insert(*it);
		}
		for(const std::string& includeDir : sysincludes) {
			if(used.contains(includeDir)) continue;
			adjArgs.push_back(std::string("-isystem") + includeDir);
			used.insert(includeDir);
		}
		for(unsigned int i = 1; i < args.size(); i++) {
			adjArgs.push_back(args[i]);
		}
		return adjArgs;
	});
	ActionFactory factory(&ctx);

	//handle macro attributes at first then
	//traverse AST, check attributes, build json objects and fill result field in context
	return (tool.run(&factory) == 0 ? std::make_optional(ctx.result) : std::nullopt);
}

Parser::~Parser() = default;
