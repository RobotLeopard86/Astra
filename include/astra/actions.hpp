#pragma once

#include "type_info.hpp"
#include "var.hpp"
#include "dll.hpp"

namespace astra {
	/**
	 * @brief A record of all action functions for a reflected type
	 */
	struct ASTRA_API Actions {
		/**
		 * @brief Create a new actions record
		 *
		 * @param reflect Takes in a pointer to an object and a const-ness flag and returns the appropriate TypeInfo
		 * @param typeName Returns the name of the type
		 * @param sizeOf Returns the size of the type
		 * @param construct Invokes the constructor to create a new object of the type in the given pointer
		 * @param destroy Invokes the destructor to clean up the object given by the pointer
		 * @param copy Copies the object at the second pointer to the first pointer
		 * @param move Moves the object at the second pointer to the first pointer
		 */
		constexpr Actions(TypeInfo (*reflect)(void*, bool), const std::string& (*typeName)(), std::size_t (*sizeOf)(), void (*construct)(void* p),
			void (*destroy)(void* p), void (*copy)(void*, const void*), void (*move)(void*, void*))
		  : reflect(reflect), typeName(typeName), sizeOf(sizeOf), construct(construct), destroy(destroy), copy(copy), move(move) {}

		constexpr Actions(const Actions& other) = default;
		constexpr Actions& operator=(const Actions& other) = default;

		TypeInfo (*reflect)(void*, bool);///<Takes in a pointer to an object and a const-ness flag and returns the appropriate TypeInfo
		const std::string& (*typeName)();///<Returns the name of the type
		std::size_t (*sizeOf)();		 ///<Returns the size of the type
		void (*construct)(void* p);		 ///<Invokes the constructor to create a new object of the type in the given pointer
		void (*destroy)(void* p);		 ///<Invokes the destructor to clean up the object given by the pointer
		void (*copy)(void*, const void*);///<Copies the object at the second pointer to the first pointer
		void (*move)(void*, void*);		 ///<Moves the object at the second pointer to the first pointer
	};

}
