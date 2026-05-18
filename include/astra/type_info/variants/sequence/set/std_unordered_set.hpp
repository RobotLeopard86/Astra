#pragma once

#include <unordered_set>

#include "../err_helper.hpp"
#include "astra/reflection/type_name.hpp"
#include "iset.hpp"

namespace astra {

	template<typename T>
	struct StdUnorderedSet : public ISet, public sequence::ErrHelper {
		StdUnorderedSet() = delete;

		StdUnorderedSet(std::unordered_set<T>* set, bool isConst)
		  : _set(set),//
			_isConst(isConst) {
		}

		Expected<None> assign(Var var) override {
			auto t = TypeId::get(_set);
			if(var.type() != t) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::typeName(var.type()),					  //
					reflection::typeName(t)));
			}

			_set = static_cast<std::unordered_set<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_set = static_cast<std::unordered_set<T>*>(ptr);
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

		size_t size() const override {
			return _set->size();
		}

		Expected<None> push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				return error("Trying to set with type: {} to unordered_set<{}>",//
					value.type(), nestedType);
			}
			_set->insert(*static_cast<const T*>(value.raw()));
			return None();
		}

		Expected<None> remove(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				return error("Cannot remove value with type: {} from unordered_set<{}>",//
					value.type(), nestedType);
			}
			_set->erase(*static_cast<const T*>(value.raw()));
			return None();
		}

		bool contains(Var value) override {
			auto it = _set->find(*static_cast<const T*>(value.raw()));
			return it != _set->end();
		}

	  private:
		std::unordered_set<T>* _set;
		bool _isConst;
	};

}
