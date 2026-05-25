#pragma once

#include <set>

#include "../err_helper.hpp"
#include "astra/type_name.hpp"
#include "iset.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdSet : public ISet, public sequence::ErrHelper {
		StdSet() = delete;

		StdSet(std::set<T>* set, bool isConst)
		  : _set(set),//
			_isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_set);
			if(var.type() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.type()),												//
					typeName(t)));
			}

			_set = static_cast<std::set<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_set = static_cast<std::set<T>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_set, TypeId::get(_set), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			for(auto&& entry : *_set) {
				//const values
				callback(Var(&entry));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *_set) {
				//const values
				callback(const_cast<T*>(&entry));
			}
		}

		void clear() override {
			_set->clear();
		}

		std::size_t size() const override {
			return _set->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				error("Trying to set with type: {} to set<{}>",//
					value.type(), nestedType);
			}
			_set->insert(*static_cast<const T*>(value.raw()));
			return;
		}

		void remove(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				error("Cannot remove value with type: {} from set<{}>",//
					value.type(), nestedType);
			}
			_set->erase(*static_cast<const T*>(value.raw()));
			return;
		}

		bool contains(Var value) override {
			auto it = _set->find(*static_cast<const T*>(value.raw()));
			return it != _set->end();
		}

	  private:
		std::set<T>* _set;
		bool _isConst;
	};

}
