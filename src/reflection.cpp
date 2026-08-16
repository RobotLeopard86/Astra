#include "astra/reflection.hpp"

#include <cstddef>

#include "astra/type_table.hpp"
#include "astra/type_actions/all_types.hpp"// IWYU pragma: keep

namespace astra {

	TypeInfo reflect(Var variable) {
		return TypeTable::actions()[variable.typeId().number()].reflect(const_cast<void*>(variable.raw()),
			variable.isConst());
	}

	const std::string& typeName(TypeId id) {
		return TypeTable::actions()[id.number()].typeName();
	}

	std::size_t sizeOf(TypeId id) {
		return TypeTable::actions()[id.number()].sizeOf();
	}

	void construct(Var variable) {
		return TypeTable::actions()[variable.typeId().number()].construct(variable.rawMut());
	}

	void destroy(Var variable) {
		if(variable.raw() == nullptr) {
		}
		TypeTable::actions()[variable.typeId().number()].destroy(variable.rawMut());
	}

	void copy(Var to, Var from) {
		if(to.isConst()) {
			throw std::runtime_error("Cannot assign to const value");
		}
		if(to.typeId() != from.typeId()) {
			throw std::runtime_error(::astra::format("Cannot copy {} to {}", typeName(from.typeId()), typeName(to.typeId())));
		}
		TypeTable::actions()[to.typeId().number()].copy(to.rawMut(), from.raw());
		return;
	}

	void move(Var to, Var from) {
		if(to.isConst()) {
			throw std::runtime_error("Cannot assign to const value");
		}
		if(to.typeId() != from.typeId()) {
			throw std::runtime_error(::astra::format("Cannot move {} to {}", typeName(from.typeId()), typeName(to.typeId())));
		}
		TypeTable::actions()[to.typeId().number()].move(to.rawMut(), from.rawMut());
		return;
	}
}