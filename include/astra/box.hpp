#pragma once

#include <memory_resource>

#include "sizeof.hpp"
#include "var.hpp"
#include "dll.hpp"

namespace astra {

	using palloc_t = std::pmr::polymorphic_allocator<uint8_t>;

	static inline palloc_t defaultAlloc;

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

		//max size of stack memory for dynamic allocation optimization
		//static const std::size_t kMemSize = Sizeof<std::unordered_map<int, int>, std::map<int, int>>::max();
		//std::array<uint8_t, kMemSize> _buff;
	};

}
