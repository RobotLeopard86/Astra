#pragma once

#include "common_actions.hpp"
#include "astra/actions_table.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API FloatActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Float(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
		requires std::is_floating_point_v<T>
	TypeId TypeId::get() {
		static TypeId id(ActionsTable::record(Actions(&FloatActions<T>::reflect,
			&CommonActions<T>::typeName,
			&CommonActions<T>::sizeOf,
			&CommonActions<T>::nop,
			&CommonActions<T>::nop,
			&CommonActions<T>::copy,
			&CommonActions<T>::move)));
		return id;
	}
}
