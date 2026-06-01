#include "astra/var.hpp"

#include "astra/reflection.hpp"
#include "astra/format.hpp"
#include "astra/types/all_types.hpp"

namespace astra {

	Var::Var()
	  : value(nullptr), type(), isThisVarConst(true) {
	}

	Var::Var(void* value, TypeId type, bool isConst)
	  : value(value), type(type), isThisVarConst(isConst) {
	}

	void Var::unsafeAssign(void* ptr) {
		value = ptr;
	}

	bool Var::operator==(const Var& other) const {
		return type == other.type && value == other.value;
	}

	bool Var::operator!=(const Var& other) const {
		return type != other.type || value != other.value;
	}

	void* Var::rawMut() const {
		if(isThisVarConst) {
			return nullptr;
		}

		return value;
	}

	const void* Var::raw() const {
		return value;
	}

	TypeId Var::typeId() const {
		return type;
	}

	bool Var::isConst() const {
		return isThisVarConst;
	}

	void Var::dispose() {
		value = nullptr;
		type = TypeId();
		isThisVarConst = true;
	}

	void Var::error(TypeId type, TypeId desired_type) {
		throw std::runtime_error(::astra::format("Cannot cast {} to {}",
			typeName(type),
			typeName(desired_type)));
	}
}