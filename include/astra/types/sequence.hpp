#pragma once

#include "common_actions.hpp"
#include "astra/reflection/the_great_table.hpp"

namespace astra {

	template<typename T>
	struct SequenceActions {
		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Sequence(static_cast<T*>(value), isConst));
		}
	};

	template<typename T>
	typename std::enable_if_t<is_sequence_v<T>, TypeId>//
	TypeId::get(T* ptr) {
		static TypeId id(TheGreatTable::record(Actions(&SequenceActions<T>::reflect,//
			&CommonActions<T>::typeName,											//
			&CommonActions<T>::typeSize,											//
			&CommonActions<T>::construct,											//
			&CommonActions<T>::destroy,												//
			&CommonActions<T>::copy,												//
			&CommonActions<T>::move)));
		return id;
	}

}
