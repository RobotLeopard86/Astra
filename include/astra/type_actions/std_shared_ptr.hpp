#pragma once

#include <memory>

#include "astra/type_table.hpp"
#include "astra/type_info.hpp"
#include "astra/type_actions/common_actions.hpp"
#include "astra/dll.hpp"
#include "astra/type_actions/type_actions.hpp"

///@cond
namespace astra {

	template<typename T>
	struct TypeActions<std::shared_ptr<T>> {

		static TypeInfo reflect(void* value, bool isConst) {
			return {Pointer(static_cast<std::shared_ptr<T>*>(value), isConst)};
		}
	};

	template<typename T>
		requires is_shared_ptr_v<T>
	TypeId TypeId::get() {
		static TypeId id(TypeTable::recordActions(Actions(&TypeActions<T>::reflect,
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
