#pragma once

#include <string_view>

#include "astra/type_id.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {
	class ASTRA_API ListErrHelper {
	  protected:
		static void error(const std::string& message, TypeId type, TypeId nestedType);
	};
}
///@endcond
