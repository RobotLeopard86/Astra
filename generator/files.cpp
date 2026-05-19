#include "files.hpp"

#include <filesystem>
#include <memory>
#include <vector>

#ifdef __linux__
#include <linux/limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#include <windows.h>

#include <algorithm>
#endif

Files::Files()
  : root(cutFilename(executableName())) {
}

#ifdef _WIN32
std::string Files::toUTF8(const wchar_t* str, std::size_t size) {
	int sizeUTF8 = WideCharToMultiByte(CP_UTF8,//
		WC_ERR_INVALID_CHARS,				   //
		str,								   //
		staticCast<int>(size),				   //
		nullptr, 0, nullptr, nullptr);

	std::string str_UTF8(size_UTF8, '\0');
	WideCharToMultiByte(CP_UTF8,//
		WC_ERR_INVALID_CHARS,	//
		str,					//
		staticCast<int>(size),	//
		strUTF8.data(),			//
		sizeUTF8,				//
		nullptr, nullptr);

	return strUTF8;
}

std::wstring Files::fromUTF8(const char* str, std::size_t size) {
	int sizeW = MultiByteToWideChar(CP_UTF8,//
		MB_ERR_INVALID_CHARS,				//
		str,								//
		staticCast<int>(size),				//
		nullptr, 0);

	std::wstring str_w(size_w, '\0');
	MultiByteToWideChar(CP_UTF8,//
		MB_ERR_INVALID_CHARS,	//
		str,					//
		staticCast<int>(size),	//
		strW.data(),			//
		sizeW);

	return strW;
}
#endif

inline std::string Files::executableName() {
#ifdef __linux__
	auto rawPath = std::make_unique<char[]>(PATH_MAX);

	auto size = readlink("/proc/self/exe", rawPath.get(), PATH_MAX);
	if(size == -1) {
		size = 0;
	}

	return std::string(rawPath.get(), size);
#elif defined(__APPLE__)
	//get size first
	uint32_t size = 0;
	auto code = _NSGetExecutablePath(nullptr, &size);

	//get the path
	auto rawPath = std::make_unique<char[]>(size);
	code = _NSGetExecutablePath(rawPath.get(), &size);
	if(code == -1) {
		size = 0;
	}

	return std::string(raw_path.get(), size);
#elif defined(_WIN32)
	std::wstring rawStr(MAX_PATH, '\0');
	//int sizeRaw = GetModuleFileNameW(nullptr, &rawStr[0], MAX_PATH);

	auto rawPath = std::make_unique<wchar_t[]>(MAX_PATH);
	auto size = GetModuleFileNameW(nullptr, rawPath.get(), MAX_PATH);

	auto pathUTF8 = toUTF8(rawPath.get(), size);

	//use '/' even on Windows
	std::replace(pathUTF8.begin(), pathUTF8.end(), '\\', '/');

	return pathUTF8;
#endif
}

void Files::completeFiles(std::vector<std::string>* paths) {
	auto old = *paths;
	paths->clear();

	for(auto path : old) {
		path = std::filesystem::canonical(path).string();

#ifdef _WIN32
		std::filesystem::path fsPath(from_UTF8(path.data(), path.size()));
#else
		std::filesystem::path fsPath(path);
#endif

		if(std::filesystem::is_directory(fsPath)) {
			for(auto&& filepath : std::filesystem::recursive_directory_iterator(fsPath)) {
#ifdef _WIN32
				auto wStr = filePath.path().wstring();
				paths->push_back(toUTF8(wStr.data(), wStr.size()));
#else
				paths->push_back(filepath.path().string());
#endif
			}
		} else {
			paths->push_back(path);
		}
	}
}

std::string Files::cutFilename(std::string str) {
	std::string ret = str;
	auto pos = ret.find_last_of(deliminator);

	if(pos != std::string::npos) {
		pos += 1;
		ret.erase(pos, ret.length() - pos);
	}

	return ret;
}

inline bool Files::isAbsolute(const std::string& path) {
#ifdef _WIN32
	return path[1] == ':';//match 'C:\', 'D:\', etc
#else
	return path.front() == deliminator;
#endif
}
