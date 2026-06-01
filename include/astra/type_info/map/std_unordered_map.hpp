#pragma once

#include <unordered_map>

#include "astra/type_name.hpp"
#include "astra/dll.hpp"
#include "imap.hpp"

namespace astra {

	template<typename K, typename V>
	struct ASTRA_API StdUnorderedMap final : public IMap {
		StdUnorderedMap() = delete;

		StdUnorderedMap(std::unordered_map<K, V>* map, bool isConst)
		  : map(map), isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(map);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			map = static_cast<std::unordered_map<K, V>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			map = static_cast<std::unordered_map<K, V>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(map, TypeId::get(map), isConst);
		}

		TypeId keyType() const override {
			return TypeId::get<K>();
		}

		TypeId valType() const override {
			return TypeId::get<V>();
		}

		void forEach(std::function<void(Var, Var)> callback) const override {
			const auto valueType = TypeId::get<V>();

			for(auto&& pair : *map) {
				callback(Var(&pair.first), Var(&pair.second, valueType, true));
			}
		}

		void forEach(std::function<void(Var, Var)> callback) override {
			const auto valueType = TypeId::get<V>();

			for(auto&& pair : *map) {
				callback(Var(&pair.first), Var(&pair.second, valueType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*, void*)> callback) const override {
			for(auto&& pair : *map) {
				callback(const_cast<K*>(&pair.first), &pair.second);
			}
		}

		void clear() override {
			map->clear();
		}

		std::size_t size() const override {
			return map->size();
		}

		void insert(Var key, Var value) override {
			auto k = key.rtCast<K>();
			auto v = value.rtCast<V>();
			auto r = map->insert(std::make_pair(*k, *v));

			if(r.second != true) {
				throw std::runtime_error("The pair of key and value does already exist");
			}

			return;
		}

		void remove(Var key) override {
			auto ptr = key.rtCast<K>();
			auto n = map->erase(*ptr);
			if(n == 0) {
				throw std::runtime_error("The element doesn't exist");
			}
		}

	  private:
		std::unordered_map<K, V>* map;
		bool isConst;
	};

}