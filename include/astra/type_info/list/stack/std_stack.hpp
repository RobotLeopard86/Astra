#pragma once

#include <stack>

#include "../err_helper.hpp"
#include "astra/type_query.hpp"
#include "istack.hpp"
#include "astra/dll.hpp"
#include "stack_iterator.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdStack : public IStack, public ListErrHelper {
		StdStack() = delete;

		StdStack(std::stack<T>* stack, bool isConst)
		  : stack(stack),
			isConst(isConst) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) override {
			auto t = TypeId::get(stack);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			stack = static_cast<std::stack<T>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) override {
			stack = static_cast<std::stack<T>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(stack, TypeId::get(stack), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();
			const auto end = StackIterator<T>::end(stack);

			for(auto it = StackIterator<T>::begin(stack); it != end; ++it) {
				callback(Var(&(*it), nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();
			const auto end = StackIterator<T>::end(stack);

			for(auto it = StackIterator<T>::begin(stack); it != end; ++it) {
				callback(Var(&(*it), nestedType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto it = StackIterator<T>::begin(stack); it != StackIterator<T>::end(stack); ++it) {
				callback(&(*it));
			}
		}

		void clear() override {
			while(!stack->empty()) {
				stack->pop();
			}
		}

		std::size_t size() const override {
			return stack->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Trying to set with type: {} to set<{}>",
					value.typeId(), nestedType);
			}
			stack->push(*static_cast<const T*>(value.raw()));
		}

		void pop() override {
			stack->pop();
		}

		Var top() override {
			if(stack->empty()) {
				throw std::runtime_error("The stack is empty");
			}

			return Var(&stack->top(), TypeId::get<T>(), isConst);
		};

	  private:
		std::stack<T>* stack;
		bool isConst;
	};

}
