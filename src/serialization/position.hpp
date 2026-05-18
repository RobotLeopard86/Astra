#pragma once

#include <cstddef>
#include <string>

#include "astra/tools/format.hpp"

namespace astra {

	struct Position {
		size_t column = 0;
		size_t lineNumber = 0;

		std::string toString() {
			return format("ln:{} col:{}", lineNumber + 1, column + 1);
		}
	};

}
