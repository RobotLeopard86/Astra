#pragma once

#include "astra/sizeof.hpp"
#include "imap.hpp"
#include "std_map.hpp"
#include "std_unordered_map.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Map final {
		Map() = delete;

		template<typename KeyT, typename ValueT>
		Map(std::map<KeyT, ValueT>* map, bool isConst) {
			new(mem) StdMap<KeyT, ValueT>(map, isConst);
		}

		template<typename KeyT, typename ValueT>
		Map(std::unordered_map<KeyT, ValueT>* map, bool isConst) {
			new(mem) StdUnorderedMap<KeyT, ValueT>(map, isConst);
		}

		void assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		Var ownVar() const {
			return impl()->ownVar();
		}

		TypeId keyType() const {
			return impl()->keyType();
		}

		TypeId valType() const {
			return impl()->valType();
		}

		void forEach(std::function<void(Var, Var)> callback) const {
			return impl()->forEach(callback);
		}

		void forEach(std::function<void(Var, Var)> callback) {
			return impl()->forEach(callback);
		}

		void unsafeForEach(std::function<void(void*, void*)> callback) const {
			return impl()->unsafeForEach(callback);
		}

		void clear() {
			impl()->clear();
		}

		std::size_t size() const {
			return impl()->size();
		}

		void insert(Var key, Var value) {
			return impl()->insert(key, value);
		}

		void remove(Var key) {
			return impl()->remove(key);
		}

	  private:
		char mem[Sizeof<StdMap<int, int>, StdUnorderedMap<int, int>>::max()];

		inline const IMap* impl() const {
			return reinterpret_cast<const IMap*>(&mem[0]);
		}

		inline IMap* impl() {
			return reinterpret_cast<IMap*>(&mem[0]);
		}
	};

}
