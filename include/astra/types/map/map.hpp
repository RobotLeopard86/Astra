#pragma once

#include "astra/sizeof.hpp"
#include "imap.hpp"
#include "std_map.hpp"
#include "std_unordered_map.hpp"
#include "astra/dll.hpp"

namespace astra {

	class ASTRA_API Map final {
	  public:
		///@cond
		Map() = delete;
		///@endcond

		/**
		 * @brief Create a new map from a @c std::map
		 *
		 * @tparam K The type of map keys
		 * @tparam V The type of map values
		 *
		 * @param map The map to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename K, typename V>
		Map(std::map<K, V>* map, bool isConst) {
			new(mem) StdMap<K, V>(map, isConst);
		}

		/**
		 * @brief Create a new map from a @c std::unordered_map
		 *
		 * @tparam K The type of map keys
		 * @tparam V The type of map values
		 *
		 * @param map The map to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename K, typename V>
		Map(std::unordered_map<K, V>* map, bool isConst) {
			new(mem) StdUnorderedMap<K, V>(map, isConst);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a map
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

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var ownVar() const {
			return impl()->ownVar();
		}

		/**
		 * @brief Get the TypeId of the list keys
		 *
		 * @return The key type ID
		 */
		TypeId keyType() const {
			return impl()->keyType();
		}

		/**
		 * @brief Get the TypeId of the list values
		 *
		 * @return The value type ID
		 */
		TypeId valType() const {
			return impl()->valType();
		}

		/**
		 * @brief Constly execute a callback on every map entry
		 *
		 * @param callback The callback to execute
		 */
		void forEach(std::function<void(Var, Var)> callback) const {
			return impl()->forEach(callback);
		}

		/**
		 * @brief Execute a callback on every map entry
		 *
		 * @param callback The callback to execute
		 */
		void forEach(std::function<void(Var, Var)> callback) {
			return impl()->forEach(callback);
		}

		/**
		 * @brief Unsafely execute a callback on every map entry
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param callback The callback to execute
		 */
		void unsafeForEach(std::function<void(void*, void*)> callback) const {
			return impl()->unsafeForEach(callback);
		}

		/**
		 * @brief Clear the whole map
		 */
		void clear() {
			impl()->clear();
		}

		/**
		 * @brief Get the current number of entries in the map
		 *
		 * @return The number of map entries
		 */
		std::size_t size() const {
			return impl()->size();
		}

		/**
		 * @brief Place a new entry into the map
		 *
		 * @param key The key to identify the entry
		 * @param value The value to associate with the key
		 */
		void insert(Var key, Var value) {
			return impl()->insert(key, value);
		}

		/**
		 * @brief Remove an entry from the map
		 *
		 * @param key The key of the entry to remove
		 */
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