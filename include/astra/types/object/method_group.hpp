#pragma once

#include <cstddef>

#include "method_iterator.hpp"
#include "astra/dll.hpp"

namespace astra {
	/**
	 * @brief Group of methods for iteration
	 */
	class ASTRA_API MethodGroup {
	  public:
		/**
		 * @brief Create a new method group
		 *
		 * @param base Mutable pointer to object
		 * @param map Map of method descriptions
		 * @param acc Access filter
		 */
		MethodGroup(void* base, const std::map<std::string_view, MethodDesc>* map, Access acc)
		  : base(base), map(map), acc(acc) {
		}

		/**
		 * @brief Create a new method group with const access
		 *
		 * @param base Const pointer to object
		 * @param map Map of method descriptions
		 * @param acc Access filter
		 */
		MethodGroup(const void* base, const std::map<std::string_view, MethodDesc>* map, Access acc)
		  : base(base), map(map), acc(acc | Access::Const) {
		}

		/**
		 * @brief Get the number of methods in the group
		 *
		 * @return Group method count
		 */
		std::size_t size() const {
			return map->size();
		}

		/**
		 * @brief Get an iterator starting at the first valid method
		 *
		 * @return The iterator
		 */
		MethodIterator begin() {
			return {map, base, acc};
		}

		/**
		 * @brief Get an iterator starting at the last possible method
		 *
		 * The mismatch in iterator types between this and begin() is intentional
		 *
		 * @return The iterator
		 */
		std::map<std::string_view, MethodDesc>::const_iterator end() {
			return map->end();
		}

	  private:
		const void* base;
		const std::map<std::string_view, MethodDesc>* map;
		const Access acc;
	};

}
