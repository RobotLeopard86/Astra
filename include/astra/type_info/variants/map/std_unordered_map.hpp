#pragma once

#include <unordered_map>

#include "astra/reflection/type_name.hpp"
#include "imap.hpp"

namespace astra {

	template<typename KeyT, typename ValueT>
	struct StdUnorderedMap final : public IMap {
		StdUnorderedMap() = delete;

		StdUnorderedMap(std::unordered_map<KeyT, ValueT>* map, bool isConst)
		  : _map(map), _isConst(isConst) {
		}

		Expected<None> assign(Var var) override {
			auto t = TypeId::get(_map);
			if(var.type() != t) {
				return Error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),						   //
					typeName(t)));
			}

			_map = static_cast<std::unordered_map<KeyT, ValueT>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_map = static_cast<std::unordered_map<KeyT, ValueT>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_map, TypeId::get(_map), _isConst);
		}

		TypeId keyType() const override {
			return TypeId::get<KeyT>();
		}

		TypeId valType() const override {
			return TypeId::get<ValueT>();
		}

		void forEach(std::function<void(Var, Var)> callback) const override {
			const auto valueType = TypeId::get<ValueT>();

			for(auto&& pair : *_map) {
				callback(Var(&pair.first), Var(&pair.second, valueType, true));
			}
		}

		void forEach(std::function<void(Var, Var)> callback) override {
			const auto valueType = TypeId::get<ValueT>();

			for(auto&& pair : *_map) {
				callback(Var(&pair.first), Var(&pair.second, valueType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*, void*)> callback) const override {
			for(auto&& pair : *_map) {
				callback(const_cast<KeyT*>(&pair.first), &pair.second);
			}
		}

		void clear() override {
			_map->clear();
		}

		std::size_t size() const override {
			return _map->size();
		}

		Expected<None> insert(Var key, Var value) override {
			auto k = key.rtCast<KeyT>();
			if(k.isError()) {
				return k.template get<Error>();
			}

			auto v = value.rtCast<ValueT>();
			if(v.isError()) {
				return v.template get<Error>();
			}

			auto r = _map->insert(std::make_pair(*k.unwrap(), *v.unwrap()));

			if(r.second != true) {
				return Error("The pair of key and value does already exist");
			}

			return None();
		}

		Expected<None> remove(Var key) override {

			auto k = key.rtCast<KeyT>();

			return k.matchMove(
				[this](KeyT* ptr) -> Expected<None> {//
					auto n = _map->erase(*ptr);

					if(n == 0) {
						return Error("The element doesn't exist");
					}

					return None();
				},
				[](Error&& err) -> Expected<None> {//
					return err;
				});
		}

	  private:
		std::unordered_map<KeyT, ValueT>* _map;
		bool _isConst;
	};

}