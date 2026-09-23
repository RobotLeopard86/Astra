#pragma once

#include "common_actions.hpp"
#include "astra/type_table.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {
	struct ASTRA_API BoolActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return {Bool(static_cast<bool*>(value), isConst)};
		}
	};

	inline TypeId TypeIdFactory<bool>::get() {
		static TypeId id(TypeTable::recordActions(Actions(&BoolActions::reflect,
			&CommonActions<bool>::typeName,
			&CommonActions<bool>::sizeOf,
			&CommonActions<bool>::nop,
			&CommonActions<bool>::nop,
			&CommonActions<bool>::copy,
			&CommonActions<bool>::move)));
		return id;
	}

}
///@endcond
