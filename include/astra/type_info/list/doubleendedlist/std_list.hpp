#pragma once

#include <list>

#include "../err_helper.hpp"
#include "astra/type_name.hpp"
#include "idoubleendedlist.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdList : public IDoubleEndedList, public list::ErrHelper {
		StdList() = delete;

		StdList(std::list<T>* list, bool isConst)
		  : _list(list),//
			_isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_list);
			if(var.type() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.type()),												//
					typeName(t)));
			}

			_list = static_cast<std::list<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_list = static_cast<std::list<T>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_list, TypeId::get(_list), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *_list) {
				callback(Var(&entry, nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *_list) {
				callback(Var(&entry, nestedType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *_list) {
				callback(&entry);
			}
		}

		void clear() override {
			_list->clear();
		}

		std::size_t size() const override {
			return _list->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				error("Trying to set with type: {} to list<{}>",//
					value.type(), nestedType);
			}
			_list->push_back(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			_list->pop_back();
		}

		void pushFront(Var element) override {
			_list->push_front(*static_cast<const T*>(element.raw()));
		}

		void popFront() override {
			_list->pop_front();
		}

		Var front() override {
			if(_list->empty()) {
				throw std::runtime_error("The list is empty");
			}
			return Var(&_list->front(), TypeId::get<T>(), _isConst);
		};

		Var back() override {
			if(_list->empty()) {
				throw std::runtime_error("The list is empty");
			}
			return Var(&_list->back(), TypeId::get<T>(), _isConst);
		};

	  private:
		std::list<T>* _list;
		bool _isConst;
	};

}
