#pragma once

#include <stack>

#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StackIterator : public std::stack<T> {
		static decltype(auto) begin(std::stack<T>* stack) {
			return (stack->*&StackIterator::c).begin();
		}

		static decltype(auto) end(std::stack<T>* stack) {
			return (stack->*&StackIterator::c).end();
		}

		template<typename SameT>
		friend struct StdStack;
	};

}
