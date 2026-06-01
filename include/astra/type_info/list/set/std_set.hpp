#pragma once

#include <set>

#include "../err_helper.hpp"
#include "astra/type_name.hpp"
#include "iset.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdSet : public ISet, public ListErrHelper {
		StdSet() = delete;

		StdSet(std::set<T>* set, bool isConst)
		  : set(set),//
			isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(set);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.typeId()),												//
					typeName(t)));
			}

			set = static_cast<std::set<T>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			set = static_cast<std::set<T>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(set, TypeId::get(set), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			for(auto&& entry : *set) {
				//const values
				callback(Var(&entry));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *set) {
				//const values
				callback(const_cast<T*>(&entry));
			}
		}

		void clear() override {
			set->clear();
		}

		std::size_t size() const override {
			return set->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Trying to set with type: {} to set<{}>",//
					value.typeId(), nestedType);
			}
			set->insert(*static_cast<const T*>(value.raw()));
			return;
		}

		void remove(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Cannot remove value with type: {} from set<{}>",//
					value.typeId(), nestedType);
			}
			set->erase(*static_cast<const T*>(value.raw()));
			return;
		}

		bool contains(Var value) override {
			auto it = set->find(*static_cast<const T*>(value.raw()));
			return it != set->end();
		}

	  private:
		std::set<T>* set;
		bool isConst;
	};

}
