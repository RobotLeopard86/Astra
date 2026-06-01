#include "astra/reflection.hpp"

#include <cstddef>
#include <iostream>
#include <memory_resource>

#include "astra/actions_table.hpp"
#include "astra/types/all_types.hpp"
#include "sprint.hpp"

namespace astra {

	TypeInfo reflect(Var variable) {
		return ActionsTable::data()[variable.typeId().number()].reflect(const_cast<void*>(variable.raw()),
			variable.isConst());
	}

	std::string sprint(const TypeInfo& info) {
		std::string result;
		sprint(info, &result, 0);
		return result;
	}

	std::string sprint(Var var) {
		return sprint(reflect(var));
	}

	void print(const TypeInfo& info) {
		std::cout << sprint(info) << std::flush;
	}

	void print(Var var) {
		print(reflect(var));
	}

	std::string_view typeName(TypeId id) {
		return ActionsTable::data()[id.number()].typeName();
	}

	std::size_t sizeOf(TypeId id) {
		return ActionsTable::data()[id.number()].sizeOf();
	}

	void construct(Var variable) {
		return ActionsTable::data()[variable.typeId().number()].construct(variable.rawMut());
	}

	void destroy(Var variable) {
		if(variable.raw() == nullptr) {
			return;
		}
		ActionsTable::data()[variable.typeId().number()].destroy(variable.rawMut());
	}

	void copy(Var to, Var from) {
		if(to.isConst()) {
			throw std::runtime_error("Cannot assign to const value");
		}
		if(to.typeId() != from.typeId()) {
			throw std::runtime_error(::astra::format("Cannot copy {} to {}", typeName(from.typeId()), typeName(to.typeId())));
		}
		ActionsTable::data()[to.typeId().number()].copy(to.rawMut(), from.raw());
		return;
	}
}