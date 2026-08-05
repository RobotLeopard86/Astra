#include "inja/function_storage.hpp"
#include "templates.hpp"

#include "files.hpp"
#include "parser.hpp"
#include "to_filename.hpp"
#include "flag.hpp"

#include "CLI11.hpp"
#include "inja/inja.hpp"// IWYU pragma: keep
#include "nlohmann/json.hpp"
#include "spinners.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>

#ifndef PROJECT_VER
#define PROJECT_VER "unknown"
#endif

#define VERBOSE_LOG(...) \
	if(!quiet) std::cout << "\x1b[2K\r" << lastMsg.str() << __VA_ARGS__ << std::endl;

#define ERROR(...) \
	std::cerr << "\x1b[0m\x1b[1;91mERROR: \x1b[0m" << __VA_ARGS__ << std::endl

#define clock std::chrono::steady_clock

inline std::stringstream lastMsg;

extern void generateSubstitutes(std::unordered_map<std::string, nlohmann::json>& results);

int main(int argc, char* argv[]) {
	//Configure CLI
	CLI::App app("Astra reflection info generator", std::filesystem::path(argv[0]).filename().string());
	std::string compDbPath;
	app.add_option("--compdb,-c", compDbPath, "Path to the directory containing compile_commands.json")->check(CLI::ExistingDirectory)->required();
	std::string outDir;
	const auto outDirValidateFunc = [](const std::string& opt) {
		auto existDir = CLI::ExistingDirectory(opt);
		auto nonexist = CLI::NonexistentPath(opt);
		if(existDir.empty() || nonexist.empty()) {
			return "";
		}
		return "Provided path exists but is not a directory";
	};
	app.add_option("--output,-o", outDir, "Path to the directory to output generated code to")->check(outDirValidateFunc)->required();
	std::string project;
	app.add_option("--project-name,-n", project, "Name of the project")->transform([](const std::string& val) { return toFilename(val); })->required();
	std::string fallbackCompiler = "";
	bool fallbackIsMsvc = false;
	const auto fallbackOptFunc = [&fallbackCompiler, &fallbackIsMsvc](const std::string& s) {
		fallbackCompiler = s;
		std::string lower = s;
		for(char& c : lower) c = (char)tolower(c);
		fallbackIsMsvc = lower.find("++") == std::string::npos && (lower.find("cl.exe") != std::string::npos || lower.find("cl") == 0);
	};
	app.add_option_function<std::string>("--fallback-compiler,-C", fallbackOptFunc, "Fallback compiler to use for system include searching if the compiler in the database is not supported (if it isn't cl.exe, a GCC-like command line is assumed)");
	std::string includePrefix = "";
	app.add_option("--include-prefix,-p", includePrefix, "Optional prefix to use for header inclusion (useful for <libname>/<header>.h formulations); do not include trailing slash");
	bool quiet = false;
	app.add_flag("--quiet,-q", quiet, "Suppress output");
	app.set_version_flag("--version,-v", []() { return PROJECT_VER; }, "Display version and exit");
	std::vector<std::string> input;
	const auto inputValidateFunc = [](const std::string& opt) {
		auto existDir = CLI::ExistingDirectory(opt);
		auto existFile = CLI::ExistingFile(opt);
		if(existDir.empty() || existFile.empty()) {
			return "";
		}
		return "Provided path does not exist or is not a directory or file";
	};
	app.add_option("input", input, "Input header files to the generator")->check(inputValidateFunc)->required();

	//Parse CLI arguments
	CLI11_PARSE(app, argc, argv);

	//Spinner to make the wait bearable
	std::unique_ptr<jms::Spinner> spinner;
	if(!quiet) {
#if defined(_WIN32) && defined(_DEBUG)
		std::cout << "\x1b[0m\x1b[1;93mWARNING:\x1b[0m You are running a debug build of Astra, data generation may be very slow!" << std::endl;
#endif
		spinner = std::make_unique<jms::Spinner>("Generating reflection data...", jms::dots);
		spinner->start();
	}

	//Correct paths
	Files files;
	compDbPath = std::filesystem::canonical(compDbPath).string();
	outDir = std::filesystem::canonical(outDir).string();
	files.completeFiles(&input);

	//If the output directory doesn't exist, we need to make it, or if it exists we need to remove old Astra contents
	std::filesystem::path out(outDir);
	if(std::filesystem::exists(out)) {
		if(std::filesystem::exists(out / "astra_generated")) std::filesystem::remove_all(out / "astra_generated");
		std::filesystem::remove(out / (project + ".astra.hpp"));
		std::filesystem::remove(out / (project + ".astra.cpp"));
	}
	std::filesystem::create_directories(out);
	VERBOSE_LOG("Prepared output directory " << out);

	//Parse source files
	clock::time_point parseBegin = clock::now();
	Parser parser(compDbPath, out.string());
	parser.findSysIncludes(input[0], fallbackCompiler, fallbackIsMsvc);
	if(!quiet && !sysincludeFailFlag.empty()) {
		spinner->finish(jms::FinishedState::FAILURE, sysincludeFailFlag);
		exit(1);
	}
	std::unordered_map<std::string, nlohmann::json> parsed;
	int counter = 0;
	for(std::string in : input) {
		//Parse this file
		std::vector<std::string> inAsVec(1);
		inAsVec[0] = in;
		auto maybeFR = parser.parse(inAsVec);
		if(!maybeFR.has_value()) {
			ERROR("Failed to parse source files due to compilation errors!");
			if(!quiet) {
				spinner->finish(jms::FinishedState::FAILURE, "Failed to generate reflection data.");
			}
			return -1;
		}
		auto fileResults = maybeFR.value();

		//Correct origins
		for(auto& [_, json] : fileResults) {
			if(!json.contains("origin") || (json.contains("origin") && json["origin"].get<std::string>().compare("") == 0)) {
				//Calculate correct origin
				auto fs_path = std::filesystem::path(in.begin(), in.end());
				std::string rel = "../";
				rel += std::filesystem::relative(fs_path, out).string();
#ifdef _WIN32
				std::replace(rel.begin(), rel.end(), '\\', '/');
#endif
				json["origin"] = rel;
			}
			json["is_substitute"] = false;
		}

		//Merge maps
		parsed.merge(fileResults);
		VERBOSE_LOG("(" << ++counter << "/" << input.size() << ") Parsed \"" << in << "\"");
	}
	clock::time_point parseEnd = clock::now();
	VERBOSE_LOG("Source parsing completed in " << (std::round(std::chrono::duration_cast<std::chrono::duration<float>>(parseEnd - parseBegin).count() * 10000) / 10000) << " seconds");

	//Serialized substitute generation
	generateSubstitutes(parsed);
	VERBOSE_LOG("Generated serialization data");

	//Set up inja
	inja::Environment inja;
	inja.add_callback("reverse", 1, [](inja::Arguments& args) {
		const nlohmann::json* src = args[0];
		nlohmann::json dest;
		for(auto it = src->rbegin(); it != src->rend(); ++it) {
			dest.push_back(*it);
		}
		return dest;
	});
	inja.add_callback("startsWith", 2, [](inja::Arguments& args) {
		const nlohmann::json* src = args[0];
		const nlohmann::json* test = args[1];
		return src->get<std::string>().starts_with(test->get<std::string>());
	});
	inja.set_trim_blocks(true);
	VERBOSE_LOG("Template engine ready");

	//Create template objects
	inja::Template headerTemplate = inja.parse(templates::Header);
	inja::Template enumTemplate = inja.parse(templates::Enum);
	inja::Template objectTemplate = inja.parse(templates::Object);
	inja::Template subTemplate = inja.parse(templates::Substitute);
	VERBOSE_LOG("Loaded templates");

	//Write root files
	clock::time_point writeBegin = clock::now();
	std::ofstream rootHeader(out / (project + ".astra.hpp"));
	if(!rootHeader.is_open()) {
		ERROR("Failed to open root header file for writing!");
		if(!quiet) {
			spinner->finish(jms::FinishedState::FAILURE, "Failed to generate reflection data.");
		}
		return -1;
	}
	rootHeader << R"(
/* ---------------------------------------- *\
|                                            |
|   Astra-generated reflection info file.    |
|               DO NOT EDIT!                 |
|                                            |
\* ---------------------------------------- */

#pragma once

#include "astra/reflection.hpp" // IWYU pragma: export
#include "astra/type_actions/all_types.hpp" // IWYU pragma: export

)";
	std::ofstream rootCpp(out / (project + ".astra.cpp"));
	if(!rootCpp.is_open()) {
		ERROR("Failed to open root implementation file for writing!");
		if(!quiet) {
			spinner->finish(jms::FinishedState::FAILURE, "Failed to generate reflection data.");
		}
		return -1;
	}
	rootCpp << R"(
/* ---------------------------------------- *\
|                                            |
|   Astra-generated reflection info file.    |
|               DO NOT EDIT!                 |
|                                            |
\* ---------------------------------------- */

#include ")"
			<< (project + ".astra.hpp") << "\"\n\n";

	//Create type reflection directory
	std::filesystem::path typesDir = out / "astra_generated";
	std::filesystem::create_directories(typesDir);

	//Write file templates
	int writeCount = (parsed.size() * 2) + 2;
	counter = 0;
	for(auto&& [objectName, json] : parsed) {
		//Generate filenames
		auto filenameUTF8 = toFilename(objectName);
		filenameUTF8 += ".astra";
#ifdef _WIN32
		auto fileName = files.fromUTF8(filenameUTF8.data(), filenameUTF8.size());

		auto hppFile = typesDir / (fileName + L".hpp");
		auto cppFile = typesDir / (fileName + L".cpp");
#else
		auto& fileName = filenameUTF8;
		auto hppFile = typesDir / (fileName + ".hpp");
		auto cppFile = typesDir / (fileName + ".cpp");
#endif

		//Ensure directories are okay
		if(!includePrefix.empty()) json["origin"] = std::format("{}/{}", includePrefix, json["origin"].get<std::string>());
		json["file_name"] = hppFile.filename();
		json["project"] = project;
		std::filesystem::create_directories(hppFile.parent_path());
		std::filesystem::create_directories(cppFile.parent_path());

		//Open file streams
		std::ofstream hpp(hppFile);
		if(!hpp.is_open()) {
			ERROR("Failed to open type header file for writing!");
			if(!quiet) {
				spinner->finish(jms::FinishedState::FAILURE, "Failed to generate reflection data.");
			}
			return -1;
		}
		std::ofstream cpp(cppFile);
		if(!cpp.is_open()) {
			ERROR("Failed to open type implementation file for writing!");
			if(!quiet) {
				spinner->finish(jms::FinishedState::FAILURE, "Failed to generate reflection data.");
			}
			return -1;
		}

		//Write generation notes
		hpp << R"(
/* ---------------------------------------- *\
|                                            |
|   Astra-generated reflection info file.    |
|               DO NOT EDIT!                 |
|                                            |
\* ---------------------------------------- */

)";
		cpp << R"(
/* ---------------------------------------- *\
|                                            |
|   Astra-generated reflection info file.    |
|               DO NOT EDIT!                 |
|                                            |
\* ---------------------------------------- */

)";

		//Render header file
		inja.render_to(hpp, headerTemplate, json);
		if(json["is_substitute"].get<bool>()) {
			inja.render_to(hpp, subTemplate, json);
		}
		hpp.close();
		VERBOSE_LOG("(" << ++counter << "/" << writeCount << ") Generated " << hppFile.generic_string());

		//Render implementation file
		if(json["kind"].get<int>() == 0) {
			inja.render_to(cpp, objectTemplate, json);
		} else {
			inja.render_to(cpp, enumTemplate, json);
		}
		cpp.close();
		VERBOSE_LOG("(" << ++counter << "/" << writeCount << ") Generated " << cppFile.generic_string());

		//Add includes to root files
		const std::string includeStr = "#include \"astra_generated/";
		rootHeader << includeStr << filenameUTF8 << ".hpp\" // IWYU pragma: export\n";
		rootCpp << includeStr << filenameUTF8 << ".cpp\" // IWYU pragma: export\n";
	}

	//Close root files
	rootHeader.close();
	VERBOSE_LOG("(" << ++counter << "/" << writeCount << ") Generated " << out / (project + ".astra.hpp"));
	rootCpp.close();
	VERBOSE_LOG("(" << ++counter << "/" << writeCount << ") Generated " << out / (project + ".astra.cpp"));
	clock::time_point writeEnd = clock::now();
	VERBOSE_LOG("File generation completed in " << (std::round(std::chrono::duration_cast<std::chrono::duration<float>>(writeEnd - writeBegin).count() * 10000) / 10000) << " seconds");

	//Write done message
	if(!quiet) {
		spinner->finish(jms::FinishedState::SUCCESS, "Generation successful!");
	}

	return 0;
}