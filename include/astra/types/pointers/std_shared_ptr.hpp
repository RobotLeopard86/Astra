#pragma once

#include <memory>
#include <stdexcept>

#include "astra/reflection/actions_table.hpp"
#include "astra/type_info/type_info.hpp"
#include "astra/type_info/variants/pointer/std_shared_ptr.hpp"
#include "astra/types/common_actions.hpp"
#include "astra/types/type_actions.hpp"

namespace astra {

	template<typename T>
	struct TypeActions<std::shared_ptr<T>> {

		static TypeInfo reflect(void* value, bool isConst) {
			return {Pointer(static_cast<std::shared_ptr<T>*>(value), isConst)};
		}
	};

	template<typename T>
	TypeId TypeId::get(std::shared_ptr<T>* /*unused*/) {
		static TypeId id(ActionsTable::record(Actions(&TypeActions<std::shared_ptr<T>>::reflect,//
			&CommonActions<std::shared_ptr<T>>::typeName,										//
			&CommonActions<std::shared_ptr<T>>::sizeOf,											//
			&CommonActions<std::shared_ptr<T>>::construct,										//
			&CommonActions<std::shared_ptr<T>>::destroy,										//
			&CommonActions<std::shared_ptr<T>>::copy,											//
			&CommonActions<std::shared_ptr<T>>::move)));
		return id;
	}

}
