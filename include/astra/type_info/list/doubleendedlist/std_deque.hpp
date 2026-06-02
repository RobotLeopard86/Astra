#pragma once

#include <deque>

#include "../err_helper.hpp"
#include "astra/type_query.hpp"
#include "idoubleendedlist.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdDeque : public IDoubleEndedList, public ListErrHelper {
		StdDeque() = delete;

		StdDeque(std::deque<T>* deque, bool isConst)
		  : deque(deque),
			isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(deque);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			deque = static_cast<std::deque<T>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			deque = static_cast<std::deque<T>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(deque, TypeId::get(deque), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *deque) {
				callback(Var(&entry, nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *deque) {
				callback(Var(&entry, nestedType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *deque) {
				callback(&entry);
			}
		}

		void clear() override {
			deque->clear();
		}

		std::size_t size() const override {
			return deque->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Trying to set with type: {} to deque<{}>",
					value.typeId(), nestedType);
			}
			deque->push_back(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			deque->pop_back();
		}

		void pushFront(Var element) override {
			deque->push_front(*static_cast<const T*>(element.raw()));
		}

		void popFront() override {
			deque->pop_front();
		}

		Var front() override {
			if(deque->empty()) {
				throw std::runtime_error("The list is empty");
			}
			return Var(&deque->front(), TypeId::get<T>(), isConst);
		};

		Var back() override {
			if(deque->empty()) {
				throw std::runtime_error("The list is empty");
			}
			return Var(&deque->back(), TypeId::get<T>(), isConst);
		};

	  private:
		std::deque<T>* deque;
		bool isConst;
	};

}
