#pragma once

#include <memory_resource>

#include "sizeof.hpp"
#include "var.hpp"
#include "dll.hpp"

namespace astra {
	///@cond
	using palloc_t = std::pmr::polymorphic_allocator<uint8_t>;
	static inline palloc_t defaultAlloc;
	///@endcond

	struct ASTRA_API Box {
		Box(palloc_t* alloc = &defaultAlloc)
		  : _alloc(alloc) {};

		Box(const Box& other) = delete;
		Box& operator=(const Box& other) = delete;

		Box(Box&& other) noexcept;
		Box& operator=(Box&& other) noexcept;

		explicit Box(TypeId id, palloc_t* alloc = &defaultAlloc);
		~Box();

		Var var();
		Box clone();

	  private:
		Var _var;
		palloc_t* _alloc;
	};

}
