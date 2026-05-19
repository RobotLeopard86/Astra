#pragma once

#include <cstddef>

namespace astra {

	template<typename T, typename... Args>
	struct Sizeof {
		static constexpr std::size_t max() {
			return sizeof(T) > Sizeof<Args...>::max() ? sizeof(T) : Sizeof<Args...>::max();
		}
	};

	template<typename T>
	struct Sizeof<T> {
		static constexpr std::size_t max() {
			return sizeof(T);
		}
	};

}
