#pragma once

#include <string_view>

#include "astra/error/error.hpp"
#include "astra/type_id.hpp"

using namespace astra;

namespace astra::sequence {

	///move error formating functionality outside sequence template classes
	class ErrHelper {
	  protected:
		static Error error(std::string_view message, TypeId type, TypeId nested_type);
	};

}