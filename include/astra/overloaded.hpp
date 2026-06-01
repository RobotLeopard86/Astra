#pragma once

#include "dll.hpp"

namespace astra {

	/**
	 * @brief Helper to wrap multiple function objects and invoke them all
	 *
	 * @tparam Ts All function types to contain
	 */
	template<typename... Ts>
	struct ASTRA_API Overloaded : Ts... {
		using Ts::operator()...;
	};

	//explicit deduction guide (not needed as of C++20)
	template<typename... Ts>
	Overloaded(Ts...) -> Overloaded<Ts...>;

}