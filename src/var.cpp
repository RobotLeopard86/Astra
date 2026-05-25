#include "astra/var.hpp"

#include "astra/reflection.hpp"
#include "astra/format.hpp"
#include "astra/types/all_types.hpp"

namespace astra {

	Var::Var()
	  : _value(nullptr), _type(), _isConst(true) {
	}

	Var::Var(void* value, TypeId type, bool isConst)
	  : _value(value), _type(type), _isConst(isConst) {
	}

	void Var::unsafeAssign(void* ptr) {
		_value = ptr;
	}

	bool Var::operator==(const Var& other) const {
		return _type == other._type && _value == other._value;
	}

	bool Var::operator!=(const Var& other) const {
		return _type != other._type || _value != other._value;
	}

	void* Var::rawMut() const {
		if(_isConst) {
			return nullptr;
		}

		return _value;
	}

	const void* Var::raw() const {
		return _value;
	}

	TypeId Var::type() const {
		return _type;
	}

	bool Var::isConst() const {
		return _isConst;
	}

	void Var::dispose() {
		_value = nullptr;
		_type = TypeId();
		_isConst = true;
	}

	void Var::error(TypeId type, TypeId desired_type) {
		throw std::runtime_error(::astra::format("Cannot cast {} to {}",//
			typeName(type),												//
			typeName(desired_type)));
	}
}