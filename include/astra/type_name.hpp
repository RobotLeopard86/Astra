#pragma once

#include "astra/type_id.hpp"
#include "astra/dll.hpp"

namespace astra {

	/**
	 * @brief Get the name of a type from its TypeId
	 *
	 * @param id The ID of the type whose name to get
	 *
	 * @return The name of the type
	 */
	ASTRA_API const std::string& typeName(TypeId id);

}