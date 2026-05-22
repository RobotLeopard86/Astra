#pragma once

#include <stack>

#include "../err_helper.hpp"
#include "astra/reflection/type_name.hpp"
#include "istack.hpp"
#include "stack_iterator.hpp"

namespace astra {

	template<typename T>
	struct StdStack : public IStack, public sequence::ErrHelper {
		StdStack() = delete;

		StdStack(std::stack<T>* stack, bool isConst)
		  : _stack(stack),//
			_isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_stack);
			if(var.type() != t) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(t)));
			}

			_stack = static_cast<std::stack<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_stack = static_cast<std::stack<T>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_stack, TypeId::get(_stack), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();
			const auto end = StackIterator<T>::end(_stack);

			for(auto it = StackIterator<T>::begin(_stack); it != end; ++it) {
				callback(Var(&(*it), nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();
			const auto end = StackIterator<T>::end(_stack);

			for(auto it = StackIterator<T>::begin(_stack); it != end; ++it) {
				callback(Var(&(*it), nestedType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto it = StackIterator<T>::begin(_stack); it != StackIterator<T>::end(_stack); ++it) {
				callback(&(*it));
			}
		}

		void clear() override {
			while(!_stack->empty()) {
				_stack->pop();
			}
		}

		std::size_t size() const override {
			return _stack->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				error("Trying to set with type: {} to set<{}>",//
					value.type(), nestedType);
			}
			_stack->push(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			_stack->pop();
		}

		Var top() override {
			if(_stack->empty()) {
				throw std::runtime_error("The stack is empty");
			}

			return Var(&_stack->top(), TypeId::get<T>(), _isConst);
		};

	  private:
		std::stack<T>* _stack;
		bool _isConst;
	};

}
