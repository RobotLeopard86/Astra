#pragma once

#include <stack>

#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T>
	class ASTRA_API StackIterator : public std::stack<T> {
	  public:
		static decltype(auto) begin(std::stack<T>* stack) {
			return (stack->*&StackIterator::c).begin();
		}

		static decltype(auto) end(std::stack<T>* stack) {
			return (stack->*&StackIterator::c).end();
		}

		template<typename U>
		friend class StdStack;
	};

}
///@endcond
