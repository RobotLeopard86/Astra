#pragma once

#include <cctype>
#include <string>

//Returns filename in snake_case
inline std::string toFilename(const std::string& str) {
	std::string name;

	name += std::tolower(str[0]);
	for(unsigned int i = 1; i < str.length(); i++) {
		char c = str[i];

		if(std::isupper(static_cast<unsigned char>(c)) != 0) {
			name += std::tolower(c);
		} else {
			if(c == ':') {
				name += '/';
				i++;
			} else if(c == '<' || c == '>') {
				name += '_';
			} else {
				name += c;
			}
		}
	}

	return name;
}