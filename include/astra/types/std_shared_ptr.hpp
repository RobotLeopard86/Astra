#pragma once

#include <memory>

#include "astra/actions_table.hpp"
#include "astra/type_info.hpp"
#include "astra/types/common_actions.hpp"
#include "astra/dll.hpp"
#include "astra/types/type_actions.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API TypeActions<std::shared_ptr<T>> {

		static TypeInfo reflect(void* value, bool isConst) {
			return {Pointer(static_cast<std::shared_ptr<T>*>(value), isConst)};
		}
	};

	template<typename T>
		requires is_shared_ptr_v<T>
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
