#include "astra/variable/box.hpp"

#include <cstdint>

#include "astra/alloc/alloc.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/types/all_types.hpp"

using namespace astra;

Box::Box(TypeId id, palloc_t* alloc)
  : _alloc(alloc) {
	_var = Var(_alloc->allocate(reflection::typeSize(id)), id, false);
	reflection::construct(_var);
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
	reflection::destroy(_var);

	auto* p = reinterpret_cast<uint8_t*>(_var.rawMut());
	auto size = reflection::typeSize(_var.type());
	_alloc->deallocate(p, size);
}

Var Box::var() {
	return _var;
}

Box Box::clone() {
	Box newOne(_var.type());
	reflection::copy(newOne.var(), var());
	return newOne;
}
