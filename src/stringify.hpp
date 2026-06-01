#pragma once

#include <iomanip>
#include <sstream>
#include <cstdint>

#include "astra/dll.hpp"

namespace astra {
	ASTRA_API inline std::string toString(bool value) {
		return value ? "true" : "false";
	}

	ASTRA_API inline std::string toString(int64_t value) {
		return std::to_string(value);
	}

	ASTRA_API inline std::string toString(uint64_t value) {
		return std::to_string(value);
	}

	ASTRA_API inline std::string toString(double value, int precision) {
		std::stringstream stream;
		stream << std::setiosflags(std::ios::fixed) << std::setprecision(precision) << value;
		return stream.str();
	}
}
