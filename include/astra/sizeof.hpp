#pragma once

#include <cstddef>

#include "dll.hpp"

namespace astra {

	template<typename T, typename... Args>
	struct ASTRA_API Sizeof {
		static constexpr std::size_t max() {
			return sizeof(T) > Sizeof<Args...>::max() ? sizeof(T) : Sizeof<Args...>::max();
		}
	};

	template<typename T>
	struct ASTRA_API Sizeof<T> {
		static constexpr std::size_t max() {
			return sizeof(T);
		}
	};

}
