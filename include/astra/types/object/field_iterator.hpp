#pragma once

#include <cstddef>
#include <map>
#include <string_view>

#include "astra/dll.hpp"
#include "field_info.hpp"

namespace astra {
	/**
	 * @brief Iterator for fields that takes access control into account
	 */
	class ASTRA_API FieldIterator {
	  public:
		///@cond
		using const_iterator = std::map<std::string_view, FieldDesc>::const_iterator;
		///@endcond

		/**
		 * @brief Create a new field iterator
		 *
		 * @param map Field map to iterate over
		 * @param base Base object to operate on
		 * @param acc Access filter for fields
		 * @param includeReadonly Whether or not to include read-only fields in the iteration
		 */
		FieldIterator(const std::map<std::string_view, FieldDesc>* map, const void* base, Access acc, bool includeReadonly)
		  : it(map->begin()), end(map->cend()), base(base), acc(acc), include_readonly(includeReadonly) {
			if(!isValid()) {
				nextValid();
			}
		}

		/**
		 * @brief Prefix increment operator
		 */
		FieldIterator& operator++() noexcept {
			nextValid();
			return *this;
		};

		/**
		 * @brief Postfix increment operator
		 */
		FieldIterator operator++(int) noexcept {
			auto t = *this;
			++(*this);
			return t;
		};

		///@cond
		bool operator==(const const_iterator& other) const noexcept {
			return it != other;
		};

		bool operator!=(const const_iterator& other) const noexcept {
			return it != other;
		};
		///@endcond

		/**
		 * @brief Access the current field
		 *
		 * @return A pair of field name and info
		 */
		std::pair<std::string_view, FieldInfo> operator*() const noexcept {
			return std::make_pair(it->first, FieldInfo(base, &it->second));
		};

	  private:
		const_iterator it;
		const const_iterator end;
		const void* base;
		const Access acc;
		const bool include_readonly;

		bool isValid() {
			return rightAccess() &&
				   rightStaticReadonly() &&
				   (base != nullptr || it->second.isStatic());
		}

		void nextValid() {
			do {
				++it;
			} while(it != end && !isValid());
		}

		inline bool rightAccess() {
			return (it->second.access() & acc & (Access::Public | Access::Protected | Access::Private)) != Access::None;
		}

		//built from Karnaugh Map
		inline bool rightStaticReadonly() {
			bool nx0 = !it->second.isReadOnly();
			bool nx1 = (it->second.access() & Access::Static) == Access::None;
			bool x2 = include_readonly;
			bool x3 = (acc & Access::Static) != Access::None;

			return (nx0 || x2) && (nx1 || x3);
		}
	};

}
