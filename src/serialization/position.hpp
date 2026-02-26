#pragma once

#include <cstddef>
#include <string>

#include "astra/tools/format.hpp"

namespace astra {

	struct Position {
		size_t column = 0;
		size_t line_number = 0;

		std::string to_string() {
			return format("ln:{} col:{}", line_number + 1, column + 1);
		}
	};

}
