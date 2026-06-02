#pragma once

#include "common_actions.hpp"
#include "astra/actions_table.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API IntActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Integer(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
		requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
	TypeId TypeId::get() {
		static TypeId id(ActionsTable::record(Actions(&IntActions<T>::reflect,
			&CommonActions<T>::typeName,
			&CommonActions<T>::sizeOf,
			&CommonActions<T>::nop,
			&CommonActions<T>::nop,
			&CommonActions<T>::copy,
			&CommonActions<T>::move)));
		return id;
	}

}
