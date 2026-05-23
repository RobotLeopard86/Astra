#pragma once

#include "astra/dll.hpp"

namespace astra {

	//just a blank struct ASTRA_API for further specialization
	template<typename T, typename = void>
	struct ASTRA_API TypeActions {};

}
