#pragma once

#include <cstddef>
#include <string>

#include "astra/format.hpp"

namespace astra {

	struct Position {
		std::size_t column = 0;
		std::size_t lineNumber = 0;

		std::string toString() {
			return ::astra::format("ln:{} col:{}", lineNumber + 1, column + 1);
		}
	};

}
