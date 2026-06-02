#pragma once

#include "reflectable.hpp"
#include "type_info.hpp"
#include "dll.hpp"

namespace astra {
	/**
	 * @brief Access the type info for a given variable
	 *
	 * @param variable The variable to reflect
	 *
	 * @return The variable's type info
	 */
	TypeInfo reflect(Var variable);

	/**
	 * @brief Access an object's type info
	 *
	 * @param pointer A pointer to the object to reflect
	 *
	 * @return The object's type info
	 */
	template<Reflectable T>
	TypeInfo reflect(T* pointer) {
		return reflect(Var(pointer));
	}

	/**
	 * @brief Access an object's type info constly
	 *
	 * @param pointer A pointer to the object to reflect
	 *
	 * @return The object's type info, with write operations disabled
	 */
	template<Reflectable T>
	TypeInfo reflect(const T* pointer) {
		return reflect(Var(pointer));
	}

	/**
	 * @brief Invoke the constructor on a variable
	 *
	 * @param variable The variable to invoke the constructor on
	 */
	ASTRA_API void construct(Var variable);

	/**
	 * @brief Invoke the denstructor on a variable
	 *
	 * @param variable The variable to invoke the denstructor on
	 */
	ASTRA_API void destroy(Var variable);

	/**
	 * @brief Copy the contents of one variable to another
	 *
	 * @throws std::runtime_error If the contained type is not copy-constructible
	 */
	ASTRA_API void copy(Var to, Var from);

	/**
	 * @brief Move the contents of one variable to another
	 *
	 * @throws std::runtime_error If the contained type is not move-constructible
	 */
	ASTRA_API void move(Var to, Var from);
}
