#pragma once

#include "type_info.hpp"
#include "var.hpp"
#include "dll.hpp"

namespace astra {

	struct ASTRA_API Actions {
		constexpr Actions(TypeInfo (*reflect)(void*, bool),//
			const std::string& (*getName)(),			   //
			std::size_t (*size)(),						   //
			void (*construct)(void* p),					   //
			void (*destroy)(void* p),					   //
			void (*copy)(void*, const void*),			   //
			void (*move)(void*, void*))
		  : reflect(reflect),	 //
			typeName(getName),	 //
			sizeOf(size),		 //
			construct(construct),//
			destroy(destroy),	 //
			copy(copy),			 //
			move(move) {
		}

		constexpr Actions(const Actions& other) = default;
		constexpr Actions& operator=(const Actions& other) = default;

		//there is no reason to move the struct

		TypeInfo (*reflect)(void*, bool);
		const std::string& (*typeName)();
		std::size_t (*sizeOf)();
		void (*construct)(void* p);
		void (*destroy)(void* p);
		void (*copy)(void*, const void*);
		void (*move)(void*, void*);
	};

}
