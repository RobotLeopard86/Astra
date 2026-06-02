#pragma once

#include "dll.hpp"
#include "type_id.hpp"

#include <cstdint>
#include <string>

namespace astra {
	/**
	 * @brief Get the name of a type from its TypeId
	 *
	 * @param id The ID of the type whose name to get
	 *
	 * @return The name of the type
	 */
	ASTRA_API const std::string& typeName(TypeId id);

	/**
	 * @brief Get the size of a type from its TypeId
	 *
	 * @param id The ID of the type whose size to get
	 *
	 * @return The name size the type
	 */
	ASTRA_API std::size_t sizeOf(TypeId id);
}