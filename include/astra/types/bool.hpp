#pragma once

#include "common_actions.hpp"
#include "astra/actions_table.hpp"
#include "astra/dll.hpp"

namespace astra {
	struct ASTRA_API BoolActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return {Bool(static_cast<bool*>(value), isConst)};
		}
	};

	inline TypeId TypeId::getBoolId() {
		static TypeId id(ActionsTable::record(Actions(&BoolActions::reflect,
			&CommonActions<bool>::typeName,
			&CommonActions<bool>::sizeOf,
			&CommonActions<bool>::nop,
			&CommonActions<bool>::nop,
			&CommonActions<bool>::copy,
			&CommonActions<bool>::move)));
		return id;
	}

}
