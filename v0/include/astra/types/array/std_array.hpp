#pragma once

#include <array>

#include "astra/reflection/actions_table.hpp"
#include "astra/types/common_actions.hpp"
#include "astra/types/type_actions.hpp"

namespace astra {

	template<typename T, std::size_t size_v>
	struct TypeActions<std::array<T, size_v>> {

		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Array(static_cast<std::array<T, size_v>*>(value), isConst));
		}
	};

	template<typename T, std::size_t size_v>
	TypeId TypeId::get(std::array<T, size_v>* /*unused*/) {
		static TypeId id(ActionsTable::record(Actions(&TypeActions<std::array<T, size_v>>::reflect,//
			&CommonActions<std::array<T, size_v>>::typeName,									   //
			&CommonActions<std::array<T, size_v>>::typeSize,									   //
			&CommonActions<std::array<T, size_v>>::construct,									   //
			&CommonActions<std::array<T, size_v>>::destroy,										   //
			&CommonActions<std::array<T, size_v>>::copy,										   //
			&CommonActions<std::array<T, size_v>>::move)));
		return id;
	}

}
