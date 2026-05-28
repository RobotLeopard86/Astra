#pragma once

#include <string_view>

#include "astra/type_id.hpp"
#include "astra/dll.hpp"
#include "astra/dll.hpp"

namespace astra {

	namespace list {

		///move error formating functionality outside list template classes
		class ASTRA_API ErrHelper {
		  protected:
			static void error(const std::string& message, TypeId type, TypeId nestedType);
		};

	}
}