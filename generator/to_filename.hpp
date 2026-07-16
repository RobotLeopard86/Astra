#pragma once

#include <cctype>
#include <string>

//Returns filename in snake_case
inline std::string toFilename(const std::string& str) {
	std::string name;

	name += std::tolower(str[0]);
	for(unsigned int i = 1; i < str.length(); i++) {
		char c = str[i];

		if(std::islower(static_cast<unsigned char>(c)) != 0 || (c >= '0' && c <= '9')) {
			name += c;
		} else if(std::isupper(static_cast<unsigned char>(c)) != 0) {
			name += std::tolower(c);
		} else if(c == ':') {
			name += '/';
			i++;
		} else if(c == ' ' || c == '>') {
			//Ignore spaces and template closing
		} else {
			name += '_';
		}
	}

	return name;
}