#pragma once

#include <deque>

#include "../err_helper.hpp"
#include "astra/reflection/type_name.hpp"
#include "ilist.hpp"

namespace astra {

	template<typename T>
	struct StdDeque : public IList, public sequence::ErrHelper {
		StdDeque() = delete;

		StdDeque(std::deque<T>* deque, bool isConst)
		  : _deque(deque),//
			_isConst(isConst) {
		}

		Expected<None> assign(Var var) override {
			auto t = TypeId::get(_deque);
			if(var.type() != t) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::typeName(var.type()),					  //
					reflection::typeName(t)));
			}

			_deque = static_cast<std::deque<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_deque = static_cast<std::deque<T>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_deque, TypeId::get(_deque), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *_deque) {
				callback(Var(&entry, nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *_deque) {
				callback(Var(&entry, nestedType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *_deque) {
				callback(&entry);
			}
		}

		void clear() override {
			_deque->clear();
		}

		size_t size() const override {
			return _deque->size();
		}

		Expected<None> push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				return error("Trying to set with type: {} to deque<{}>",//
					value.type(), nestedType);
			}
			_deque->push_back(*static_cast<const T*>(value.raw()));
			return None();
		}

		void pop() override {
			_deque->pop_back();
		}

		void pushFront(Var element) override {
			_deque->push_front(*static_cast<const T*>(element.raw()));
		}

		void popFront() override {
			_deque->pop_front();
		}

		Expected<Var> front() override {
			if(_deque->empty()) {
				return Error("The list is empty");
			}
			return Var(&_deque->front(), TypeId::get<T>(), _isConst);
		};

		Expected<Var> back() override {
			if(_deque->empty()) {
				return Error("The list is empty");
			}
			return Var(&_deque->back(), TypeId::get<T>(), _isConst);
		};

	  private:
		std::deque<T>* _deque;
		bool _isConst;
	};

}
