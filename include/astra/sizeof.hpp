#pragma once

#include <cstddef>
#include <algorithm>

#include "dll.hpp"

namespace astra {
	/**
	 * @brief Helper for finding the size of the largest type in Ts
	 *
	 * @tparam Ts The types to compare
	 */
	template<typename... Ts>
	struct ASTRA_API Sizeof {
		/**
		 * @brief Get the maximum size needed to hold any of the types in Ts
		 *
		 * @return The max size
		 */
		static constexpr std::size_t max() {
			return std::max({sizeof(Ts)...});
		}
	};
}
