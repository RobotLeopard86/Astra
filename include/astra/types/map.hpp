#pragma once

#include "common_actions.hpp"
#include "astra/reflection/the_great_table.hpp"

namespace astra {

	template<typename T>
	struct MapActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Map(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
	typename std::enable_if_t<is_map_v<T>, TypeId>//
	TypeId::get(T* ptr) {
		static TypeId id(TheGreatTable::record(Actions(&MapActions<T>::reflect,//
			&CommonActions<T>::typeName,									   //
			&CommonActions<T>::typeSize,									   //
			&CommonActions<T>::construct,									   //
			&CommonActions<T>::destroy,										   //
			&CommonActions<T>::copy,										   //
			&CommonActions<T>::move)));
		return id;
	}

}
