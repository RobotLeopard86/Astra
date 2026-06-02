#pragma once

#include <cstddef>
#include <map>
#include <string_view>

#include "method_info.hpp"
#include "astra/dll.hpp"

namespace astra {
	/**
	 * @brief Iterator for methods that takes access control into account
	 */
	class ASTRA_API MethodIterator {
	  public:
		using const_iterator = std::map<std::string_view, MethodDesc>::const_iterator;
		using item = std::pair<std::string_view, MethodInfo>;

		/**
		 * @brief Create a new method iterator
		 *
		 * @param map Method map to iterate over
		 * @param base Base object to operate on
		 * @param acc Access filter for fields
		 */
		MethodIterator(const std::map<std::string_view, MethodDesc>* map, const void* base, Access acc)
		  : it(map->begin()), end(map->cend()), base(base), acc(acc) {
			if(!isValid()) {
				nextValid();
			}
		}

		/**
		 * @brief Prefix increment operator
		 */
		MethodIterator& operator++() noexcept {
			nextValid();
			return *this;
		};

		/**
		 * @brief Postfix increment operator
		 */
		MethodIterator operator++(int) noexcept {
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
		 * @brief Access the current method
		 *
		 * @return A pair of method name and info
		 */
		std::pair<std::string_view, MethodInfo> operator*() const noexcept {
			return std::make_pair(it->first, MethodInfo(base, &it->second));
		};

	  private:
		const_iterator it;
		const const_iterator end;
		const void* base;
		const Access acc;

		bool isValid() {
			return rightAccess() && (base != nullptr || it->second.isStatic());
		}

		void nextValid() {
			do {
				++it;
			} while(!isValid() && it != end);
		}

		inline bool rightAccess() {
			return (it->second.access() & acc & (Access::Public | Access::Protected | Access::Private)) != Access::None;
		}
	};

}
