#pragma once

#include "common_actions.hpp"
#include "astra/reflection/actions_table.hpp"

namespace astra {

	struct BoolActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return {Bool(static_cast<bool*>(value), isConst)};
		}
	};

	template<>
	inline TypeId TypeId::get(bool* /*unused*/) {
		static TypeId id(ActionsTable::record(Actions(&BoolActions::reflect,//
			&CommonActions<bool>::typeName,									//
			&CommonActions<bool>::sizeOf,									//
			&CommonActions<bool>::nop,										//
			&CommonActions<bool>::nop,										//
			&CommonActions<bool>::copy,										//
			&CommonActions<bool>::move)));
		return id;
	}

}
