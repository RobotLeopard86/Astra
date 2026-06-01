#pragma once

#include "reflectable.hpp"
#include "type_id.hpp"
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
	 * @brief Access an object's type info constly
	 *
	 * @param pointer A pointer to the object to reflect
	 *
	 * @return The object's type info, with write operations disabled
	 */
	std::string sprint(const TypeInfo& info);
	std::string sprint(Var var);

	template<Reflectable T>
	std::string sprint(const T* pointer) {
		return sprint(reflect(pointer));
	}

	void print(const TypeInfo& info);
	void print(Var var);

	template<Reflectable T>
	void print(const T* pointer) {
		print(reflect(pointer));
	}

	std::size_t sizeOf(TypeId id);

	void construct(Var variable);
	void destroy(Var variable);

	void copy(Var to, Var from);

}
