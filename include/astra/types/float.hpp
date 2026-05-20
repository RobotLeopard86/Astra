#pragma once

#include "common_actions.hpp"
#include "astra/reflection/actions_table.hpp"

namespace astra {

	template<typename T>
	struct FloatActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Floating(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
	typename std::enable_if_t<std::is_floating_point_v<T>, TypeId>//
	TypeId::get(T* ptr) {
		static TypeId id(ActionsTable::record(Actions(&FloatActions<T>::reflect,//
			&CommonActions<T>::typeName,										//
			&CommonActions<T>::typeSize,										//
			&CommonActions<T>::nop,												//
			&CommonActions<T>::nop,												//
			&CommonActions<T>::copy,											//
			&CommonActions<T>::move)));
		return id;
	}

}
