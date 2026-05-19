#pragma once

#include <vector>
#include <string>

class Files {
  public:
	Files();

#ifdef _WIN32
	std::string toUTF8(const wchar_t* str, std::size_t size);
	std::wstring fromUTF8(const char* str, std::size_t size);
#endif

	const std::string root;
	static constexpr char deliminator = '/';

	std::string executableName();

	//change path to a directory to paths to files inside
	void completeFiles(std::vector<std::string>* paths);

	std::string cutFilename(std::string str);
	bool isAbsolute(const std::string& path);
};
