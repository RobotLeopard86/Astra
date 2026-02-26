#pragma once

#include <array>

#include "astra/reflection/the_great_table.hpp"
#include "astra/types/common_actions.hpp"
#include "astra/types/type_actions.hpp"

namespace astra {

	template<typename T, size_t size_v>
	struct TypeActions<std::array<T, size_v>> {

		static TypeInfo reflect(void* value, bool is_const) {
			return TypeInfo(Array(static_cast<std::array<T, size_v>*>(value), is_const));
		}
	};

	template<typename T, size_t size_v>
	TypeId TypeId::get(std::array<T, size_v>* /*unused*/) {
		static TypeId id(TheGreatTable::record(Actions(&TypeActions<std::array<T, size_v>>::reflect,//
			&CommonActions<std::array<T, size_v>>::type_name,										//
			&CommonActions<std::array<T, size_v>>::type_size,										//
			&CommonActions<std::array<T, size_v>>::construct,										//
			&CommonActions<std::array<T, size_v>>::destroy,											//
			&CommonActions<std::array<T, size_v>>::copy,											//
			&CommonActions<std::array<T, size_v>>::move)));
		return id;
	}

}
