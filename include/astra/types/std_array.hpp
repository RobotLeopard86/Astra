#pragma once

#include <array>

#include "astra/actions_table.hpp"
#include "astra/traits.hpp"
#include "astra/types/common_actions.hpp"
#include "astra/types/type_actions.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T, std::size_t size_v>
	struct ASTRA_API TypeActions<std::array<T, size_v>> {

		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Array(static_cast<std::array<T, size_v>*>(value), isConst));
		}
	};

	template<typename T>
		requires is_std_array_v<T>
	TypeId TypeId::get() {
		static TypeId id(ActionsTable::record(Actions(&TypeActions<T>::reflect,
			&CommonActions<T>::typeName,
			&CommonActions<T>::sizeOf,
			&CommonActions<T>::construct,
			&CommonActions<T>::destroy,
			&CommonActions<T>::copy,
			&CommonActions<T>::move)));
		return id;
	}

}

///@endcond
