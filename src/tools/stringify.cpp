#include "astra/tools/stringify.hpp"

#include <iomanip>
#include <sstream>

namespace astra {

	std::string toString(bool value) {
		return value ? "true" : "false";
	}

	std::string toString(int64_t value) {
		return std::to_string(value);
	}

	std::string toString(uint64_t value) {
		return std::to_string(value);
	}

	std::string toString(double value, int precision) {
		std::stringstream stream;
		stream << std::setiosflags(std::ios::fixed) << std::setprecision(precision) << value;

		return stream.str();
	}

}