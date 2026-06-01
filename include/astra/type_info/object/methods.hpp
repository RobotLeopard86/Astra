#pragma once

#include <cstddef>

#include "method_iterator.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Methods {
		Methods(void* base, const std::map<std::string_view, MethodDesc>* map, Access acc)
		  : base(base), map(map), acc(acc) {
		}

		std::size_t size() const {
			return map->size();
		}

		MethodIterator begin() {
			return {map, base, acc};
		}

		//iterator type is not the same as begin
		std::map<std::string_view, MethodDesc>::const_iterator end() {
			return map->end();
		}

	  private:
		const void* base;
		const std::map<std::string_view, MethodDesc>* map;
		const Access acc;
	};

}
