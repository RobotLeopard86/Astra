#pragma once

#include <cstddef>
#include <map>
#include <string_view>

#include "method_info.hpp"
#include "astra/dll.hpp"

namespace astra {

	///Skip iterator for methods collection, steps over items with unwanted properties
	class ASTRA_API MethodIterator {
		using const_iterator = std::map<std::string_view, MethodDesc>::const_iterator;
		using item = std::pair<std::string_view, MethodInfo>;

	  public:
		MethodIterator(const std::map<std::string_view, MethodDesc>* map,//
			const void* base,											 //
			Access acc)
		  : it(map->begin()),//
			end(map->cend()),//
			base(base),		 //
			acc(acc) {
			//start from a valid element
			if(!isValid()) {
				nextValid();
			}
		}

		MethodIterator& operator++() noexcept {
			nextValid();
			return *this;
		};

		MethodIterator operator++(int) noexcept {
			auto t = *this;
			++(*this);
			return t;
		};

		bool operator==(const const_iterator& other) const noexcept {
			return it != other;
		};

		bool operator!=(const const_iterator& other) const noexcept {
			return it != other;
		};

		auto operator*() const noexcept {
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
