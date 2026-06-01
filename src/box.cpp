#include "astra/box.hpp"

#include <cstdint>

#include "astra/reflection.hpp"
#include "astra/types/all_types.hpp"

namespace astra {

	Box::Box(TypeId id, palloc_t* alloc)
	  : alloc(alloc) {
		inner = Var(alloc->allocate(sizeOf(id)), id, false);
		construct(inner);
	}

	Box::Box(Box&& other) noexcept
	  : inner(other.inner), alloc(other.alloc) {
		//prevent resource deletion
		other.inner.dispose();
	}

	Box& Box::operator=(Box&& other) noexcept {
		inner = other.inner;
		alloc = other.alloc;

		//prevent resource deletion
		other.inner.dispose();
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