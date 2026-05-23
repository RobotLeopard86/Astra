#pragma once

#include <iomanip>
#include <sstream>
#include <cstdint>

#include "dll.hpp"

namespace astra {

	ASTRA_API std::string toString(bool value);
	ASTRA_API std::string toString(int64_t value);
	ASTRA_API std::string toString(uint64_t value);
	ASTRA_API std::string toString(double value, int precision);

}
