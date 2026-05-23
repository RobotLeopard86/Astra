#pragma once

#include <string_view>

#include "astra/type_id.hpp"
#include "astra/dll.hpp"
#include "astra/dll.hpp"

namespace astra {

	namespace sequence {

		///move error formating functionality outside sequence template classes
		class ASTRA_API ErrHelper {
		  protected:
			static void error(std::string_view message, TypeId type, TypeId nestedType);
		};

	}
}