#pragma once

#include "astra/sizeof.hpp"
#include "imap.hpp"
#include "std_map.hpp"
#include "std_unordered_map.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Map final {
		Map() = delete;

		template<typename K, typename V>
		Map(std::map<K, V>* map, bool isConst) {
			new(mem) StdMap<K, V>(map, isConst);
		}

		template<typename K, typename V>
		Map(std::unordered_map<K, V>* map, bool isConst) {
			new(mem) StdUnorderedMap<K, V>(map, isConst);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) {
			return impl()->assign(var);
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
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
