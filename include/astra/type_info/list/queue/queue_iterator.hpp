#pragma once

#include <queue>

#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API QueueIterator : public std::queue<T> {
		static decltype(auto) begin(std::queue<T>* queue) {
			return (queue->*&QueueIterator::c).begin();
		}

		static decltype(auto) end(std::queue<T>* queue) {
			return (queue->*&QueueIterator::c).end();
		}

		template<typename U>
		friend struct StdQueue;
	};

}
