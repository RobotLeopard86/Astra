#include "astra/box.hpp"

#include <cstdint>
#include <memory_resource>

#include "astra/reflection.hpp"
#include "astra/type_actions/all_types.hpp"

namespace astra {

	Box::Box(TypeId id, std::pmr::polymorphic_allocator<uint8_t>* alloc)
	  : alloc(alloc) {
		inner = Var(alloc->allocate(sizeOf(id)), id, false);
		construct(inner);
	}

	Box::Box(Box&& other) noexcept
	  : inner(other.inner), alloc(other.alloc) {
		other.inner.release();
	}

	Box& Box::operator=(Box&& other) noexcept {
		inner = other.inner;
		alloc = other.alloc;
		other.inner.release();
		return *this;
	}

	Box::~Box() {
		destroy(inner);

		auto* p = reinterpret_cast<uint8_t*>(inner.rawMut());
		auto size = sizeOf(inner.typeId());
		alloc->deallocate(p, size);
	}

	Var Box::var() {
		return inner;
	}

	Box Box::clone() {
		Box cloned(inner.typeId());
		copy(cloned.var(), var());
		return cloned;
	}
}