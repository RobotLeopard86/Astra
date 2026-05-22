#pragma once

#include <queue>

#include "../err_helper.hpp"
#include "astra/reflection/type_name.hpp"
#include "astra/variant/variant.hpp"
#include "iqueue.hpp"
#include "queue_iterator.hpp"

namespace astra {

	template<typename T>
	struct StdQueue : public IQueue, public sequence::ErrHelper {
		StdQueue() = delete;

		StdQueue(std::queue<T>* queue, bool isConst)
		  : _queue(queue),//
			_isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_queue);
			if(var.type() != t) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(t)));
			}

			_queue = static_cast<std::queue<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_queue = static_cast<std::queue<T>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_queue, TypeId::get(_queue), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();
			const auto end = QueueIterator<T>::end(_queue);

			for(auto it = QueueIterator<T>::begin(_queue); it != end; ++it) {
				callback(Var(&(*it), nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();
			const auto end = QueueIterator<T>::end(_queue);

			for(auto it = QueueIterator<T>::begin(_queue); it != end; ++it) {
				callback(Var(&(*it), nestedType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			const auto end = QueueIterator<T>::end(_queue);

			for(auto it = QueueIterator<T>::begin(_queue); it != end; ++it) {
				callback(&(*it));
			}
		}

		void clear() override {
			while(!_queue->empty()) {
				_queue->pop();
			}
		}

		std::size_t size() const override {
			return _queue->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				error("Trying to set with type: {} to queue<{}>",//
					value.type(), nestedType);
			}
			_queue->push(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			_queue->pop();
		}

		Var front() override {
			return Var(&_queue->front());
		};

		Var back() override {
			return Var(&_queue->back());
		};

	  private:
		std::queue<T>* _queue;
		bool _isConst;
	};

}
