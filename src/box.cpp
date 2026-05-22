#include "astra/box.hpp"

#include <cstdint>

#include "astra/reflection.hpp"
#include "astra/types/all_types.hpp"

namespace astra {

	Box::Box(TypeId id, palloc_t* alloc)
	  : _alloc(alloc) {
		_var = Var(_alloc->allocate(sizeOf(id)), id, false);
		construct(_var);
	}

	Box::Box(Box&& other) noexcept
	  : _var(other._var), _alloc(other._alloc) {
		//prevent resource deletion
		other._var.dispose();
	}

	Box& Box::operator=(Box&& other) noexcept {
		_var = other._var;
		_alloc = other._alloc;

		//prevent resource deletion
		other._var.dispose();
		return *this;
	}

	Box::~Box() {
		destroy(_var);

		auto* p = reinterpret_cast<uint8_t*>(_var.rawMut());
		auto size = sizeOf(_var.type());
		_alloc->deallocate(p, size);
	}

	Var Box::var() {
		return _var;
	}

	Box Box::clone() {
		Box newOne(_var.type());
		copy(newOne.var(), var());
		return newOne;
	}
}