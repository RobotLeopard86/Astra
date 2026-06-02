#pragma once

#include "common_actions.hpp"
#include "astra/actions_table.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API ListActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(List(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
		requires is_list_v<T>
	TypeId TypeId::get() {
		static TypeId id(ActionsTable::record(Actions(&ListActions<T>::reflect,
			&CommonActions<T>::typeName,
			&CommonActions<T>::sizeOf,
			&CommonActions<T>::construct,
			&CommonActions<T>::destroy,
			&CommonActions<T>::copy,
			&CommonActions<T>::move)));
		return id;
	}
}
