#include "astra/reflection/reflection.hpp"

#include <cstddef>
#include <iostream>
#include <memory_resource>

#include "astra/reflection/actions_table.hpp"
#include "astra/types/all_types.hpp"
#include "sprint.hpp"

namespace astra {

	TypeInfo reflect(Var variable) {
		return ActionsTable::data()[variable.type().number()].reflect(const_cast<void*>(variable.raw()),
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

#ifdef _DEBUG
	std::string_view typeName(uint32_t id) {
		return ActionsTable::data()[id].typeName();
	}
#endif

	std::size_t sizeOf(TypeId id) {
		return ActionsTable::data()[id.number()].sizeOf();
	}

	void construct(Var variable) {
		return ActionsTable::data()[variable.type().number()].construct(variable.rawMut());
	}

	void destroy(Var variable) {
		if(variable.raw() == nullptr) {
			return;
		}
		ActionsTable::data()[variable.type().number()].destroy(variable.rawMut());
	}

	void copy(Var to, Var from) {
		if(to.isConst()) {
			throw std::runtime_error("Cannot assign to const value");
		}
		if(to.type() != from.type()) {
			throw std::runtime_error(astra::format("Cannot copy {} to {}", typeName(from.type()), typeName(to.type())));
		}
		ActionsTable::data()[to.type().number()].copy(to.rawMut(), from.raw());
		return;
	}
}