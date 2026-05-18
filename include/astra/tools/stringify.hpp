#pragma once

#include <iomanip>
#include <sstream>
#include <cstdint>

namespace astra {

	std::string toString(bool value);
	std::string toString(int64_t value);
	std::string toString(uint64_t value);
	std::string toString(double value, int precision);

}
