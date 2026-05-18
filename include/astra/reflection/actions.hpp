#pragma once

#include "astra/type_info/type_info.hpp"
#include "astra/variable/var.hpp"

namespace astra {

	struct Actions {
		constexpr Actions(TypeInfo (*reflect)(void*, bool),//
			std::string_view (*getName)(),				   //
			size_t (*size)(),							   //
			void (*construct)(void* p),					   //
			void (*destroy)(void* p),					   //
			void (*copy)(void*, const void*),			   //
			void (*move)(void*, void*))
		  : reflect(reflect),	 //
			typeName(getName),	 //
			typeSize(size),		 //
			construct(construct),//
			destroy(destroy),	 //
			copy(copy),			 //
			move(move) {
		}

		constexpr Actions(const Actions& other) = default;
		constexpr Actions& operator=(const Actions& other) = default;

		//there is no reason to move the struct

		TypeInfo (*reflect)(void*, bool);
		std::string_view (*typeName)();
		size_t (*typeSize)();
		void (*construct)(void* p);
		void (*destroy)(void* p);
		void (*copy)(void*, const void*);
		void (*move)(void*, void*);
	};

}
