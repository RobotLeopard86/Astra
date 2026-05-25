#pragma once

#include "dll.hpp"

namespace astra {

	//a little helper for std::variant
	template<typename... Ts>
	struct ASTRA_API Overloaded : Ts... {
		using Ts::operator()...;
	};

	//explicit deduction guide (not needed as of C++20)
	template<typename... Ts>
	Overloaded(Ts...) -> Overloaded<Ts...>;

}