#pragma once

#include <queue>

#include "../err_helper.hpp"
#include "astra/type_name.hpp"
#include "astra/variant.hpp"
#include "iqueue.hpp"
#include "queue_iterator.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdQueue : public IQueue, public ListErrHelper {
		StdQueue() = delete;

		StdQueue(std::queue<T>* queue, bool isConst)
		  : queue(queue),
			isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(queue);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			queue = static_cast<std::queue<T>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			queue = static_cast<std::queue<T>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(queue, TypeId::get(queue), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();
			const auto end = QueueIterator<T>::end(queue);

			for(auto it = QueueIterator<T>::begin(queue); it != end; ++it) {
				callback(Var(&(*it), nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();
			const auto end = QueueIterator<T>::end(queue);

			for(auto it = QueueIterator<T>::begin(queue); it != end; ++it) {
				callback(Var(&(*it), nestedType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			const auto end = QueueIterator<T>::end(queue);

			for(auto it = QueueIterator<T>::begin(queue); it != end; ++it) {
				callback(&(*it));
			}
		}

		void clear() override {
			while(!queue->empty()) {
				queue->pop();
			}
		}

		std::size_t size() const override {
			return queue->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Trying to set with type: {} to queue<{}>",
					value.typeId(), nestedType);
			}
			queue->push(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			queue->pop();
		}

		Var front() override {
			return Var(&queue->front());
		};

		Var back() override {
			return Var(&queue->back());
		};

	  private:
		std::queue<T>* queue;
		bool isConst;
	};

}
