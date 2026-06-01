#pragma once

#include <list>

#include "../err_helper.hpp"
#include "astra/type_name.hpp"
#include "idoubleendedlist.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdList : public IDoubleEndedList, public ListErrHelper {
		StdList() = delete;

		StdList(std::list<T>* list, bool isConst)
		  : list(list),//
			isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(list);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.typeId()),												//
					typeName(t)));
			}

			list = static_cast<std::list<T>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			list = static_cast<std::list<T>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(list, TypeId::get(list), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *list) {
				callback(Var(&entry, nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *list) {
				callback(Var(&entry, nestedType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *list) {
				callback(&entry);
			}
		}

		void clear() override {
			list->clear();
		}

		std::size_t size() const override {
			return list->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Trying to set with type: {} to list<{}>",//
					value.typeId(), nestedType);
			}
			list->push_back(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			list->pop_back();
		}

		void pushFront(Var element) override {
			list->push_front(*static_cast<const T*>(element.raw()));
		}

		void popFront() override {
			list->pop_front();
		}

		Var front() override {
			if(list->empty()) {
				throw std::runtime_error("The list is empty");
			}
			return Var(&list->front(), TypeId::get<T>(), isConst);
		};

		Var back() override {
			if(list->empty()) {
				throw std::runtime_error("The list is empty");
			}
			return Var(&list->back(), TypeId::get<T>(), isConst);
		};

	  private:
		std::list<T>* list;
		bool isConst;
	};

}
