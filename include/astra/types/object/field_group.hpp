#pragma once

#include <cstddef>

#include "field_iterator.hpp"

#include "astra/dll.hpp"

namespace astra {

	class ASTRA_API FieldGroup {
	  public:
		/**
		 * @brief Create a new field group
		 *
		 * @param base Mutable pointer to object
		 * @param map Map of field descriptions
		 * @param access Access filter
		 * @param includeReadonly Whether or not to include read-only fields in the group
		 */
		FieldGroup(void* base, const std::map<std::string_view, FieldDesc>* map, Access access, bool includeReadonly)
		  : base(base), map(map), access(access), includeReadOnly(includeReadonly) {
		}

		/**
		 * @brief Create a new field group with const access
		 *
		 * @param base Const pointer to object
		 * @param map Map of field descriptions
		 * @param access Access filter
		 * @param includeReadonly Whether or not to include read-only fields in the group
		 */
		FieldGroup(const void* base, const std::map<std::string_view, FieldDesc>* map, Access access, bool includeReadonly)
		  : base(base), map(map), access(access | Access::Const), includeReadOnly(includeReadonly) {
		}

		/**
		 * @brief Get the number of fields in the group
		 *
		 * @return Group field count
		 */
		std::size_t size() const {
			return map->size();
		}

		/**
		 * @brief Get an iterator starting at the first valid field
		 *
		 * @return The iterator
		 */
		FieldIterator begin() {
			return {map, base, access, includeReadOnly};
		}

		/**
		 * @brief Get an iterator starting at the last possible field
		 *
		 * The mismatch in iterator types between this and begin() is intentional
		 *
		 * @return The iterator
		 */
		std::map<std::string_view, FieldDesc>::const_iterator end() {
			return map->end();
		}

	  private:
		const void* base;
		const std::map<std::string_view, FieldDesc>* map;
		const Access access;
		const bool includeReadOnly;
	};

}