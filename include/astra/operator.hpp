#pragma once

#include "dll.hpp"

#include <string>

namespace astra {
	/**
	 * @brief A struct containing function references to operate on a reflected object
	 *
	 * @warning This struct operates using raw void pointers and is not recommended for external use. Be careful!
	 */
	struct ASTRA_API Operator {
	  public:
		//Constexpr constructor
		constexpr Operator(const std::string& (*typeName)(), std::size_t (*sizeOf)(), void* (*construct)(), void (*destroy)(void*), void (*copy)(void*, const void*), void (*move)(void*, void*))
		  : typeName(typeName), sizeOf(sizeOf), construct(construct), destroy(destroy), copy(copy), move(move) {}

		//Constexpr copy
		constexpr Operator(const Operator& other) = default;
		constexpr Operator& operator=(const Operator& other) = default;

		const std::string& (*typeName)();///<Returns the name of the type
		std::size_t (*sizeOf)();		 ///<Returns the size of the type
		void* (*construct)();			 ///<Constructs a new object and returns the pointer
		void (*destroy)(void*);			 ///<Destroys the object, but <b>does not free associated memory</b>
		void (*copy)(void*, const void*);///<Copies the object to another object of the same type
		void (*move)(void*, void*);		 ///<Moves the object to another object of the same type
	};
}