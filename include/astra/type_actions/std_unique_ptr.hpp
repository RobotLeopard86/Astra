#pragma once

#include <memory>

#include "astra/actions_table.hpp"
#include "astra/type_actions/common_actions.hpp"
#include "astra/type_actions/type_actions.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T>
	struct ASTRA_API TypeActions<std::unique_ptr<T>> {

		static TypeInfo reflect(void* value, bool isConst) {
			return {Pointer(static_cast<std::unique_ptr<T>*>(value), isConst)};
		}

		static void copy(void* to, const void* from) {
			//move instead of copy
			auto nc_ptr = static_cast<std::unique_ptr<T>*>(const_cast<void*>(from));
			*static_cast<std::unique_ptr<T>*>(to) = std::move(*nc_ptr);
		}
	};

	template<typename T>
		requires is_unique_ptr_v<T>
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
