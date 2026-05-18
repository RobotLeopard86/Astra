#pragma once

#include "common_actions.hpp"
#include "astra/reflection/the_great_table.hpp"

namespace astra {

	template<typename T>
	struct IntActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Integer(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
	typename std::enable_if_t<std::is_integral_v<T>, TypeId>//
	TypeId::get(T* ptr) {
		static TypeId id(TheGreatTable::record(Actions(&IntActions<T>::reflect,//
			&CommonActions<T>::typeName,									   //
			&CommonActions<T>::typeSize,									   //
			&CommonActions<T>::nop,											   //
			&CommonActions<T>::nop,											   //
			&CommonActions<T>::copy,										   //
			&CommonActions<T>::move)));
		return id;
	}

}
