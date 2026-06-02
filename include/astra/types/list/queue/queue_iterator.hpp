#pragma once

#include <queue>

#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T>
	class ASTRA_API QueueIterator : public std::queue<T> {
	  public:
		static decltype(auto) begin(std::queue<T>* queue) {
			return (queue->*&QueueIterator::c).begin();
		}

		static decltype(auto) end(std::queue<T>* queue) {
			return (queue->*&QueueIterator::c).end();
		}

		template<typename U>
		friend class StdQueue;
	};

}
///@endcond
