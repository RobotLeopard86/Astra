#pragma once

#include <cstddef>

#include "astra/actions_table.hpp"
#include "astra/names.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API CommonActions {
		static std::string_view typeName() {
			static auto name = Names<T>::get();
			return name;
		}

		static std::size_t sizeOf() {
			return sizeof(T);
		}

		static void construct(void* p) {
			new(p) T();
		}

		static void destroy(void* p) {
			static_cast<T*>(p)->~T();
		}

		static void nop(void* p) {
		}

		static void copy(void* to, const void* from) {
			*static_cast<T*>(to) = *static_cast<const T*>(from);
		}

		static void move(void* to, void* from) {
			*static_cast<T*>(to) = std::move(*static_cast<T*>(from));
		}
	};

}
