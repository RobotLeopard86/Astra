#pragma once

#include "common_actions.hpp"
#include "astra/actions_table.hpp"

namespace astra {

	template<typename T>
	struct StringActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(String(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
	typename std::enable_if_t<is_string_v<T>, TypeId>//
	TypeId::get(T* ptr) {
		static TypeId id(ActionsTable::record(Actions(&StringActions<T>::reflect,//
			&CommonActions<T>::typeName,										 //
			&CommonActions<T>::sizeOf,											 //
			&CommonActions<T>::construct,										 //
			&CommonActions<T>::destroy,											 //
			&CommonActions<T>::copy,											 //
			&CommonActions<T>::move)));
		return id;
	}

}
