#pragma once

#include <memory_resource>

namespace astra {

	using palloc_t = std::pmr::polymorphic_allocator<uint8_t>;

	static inline palloc_t default_alloc;

}
