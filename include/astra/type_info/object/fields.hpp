#pragma once

#include <cstddef>

#include "field_iterator.hpp"

#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Fields {
		Fields(const void* base, const std::map<std::string_view, FieldDesc>* map, Access access, bool includeReadonly)
		  : base(base), map(map), access(access), include_readonly(includeReadonly) {
		}

		std::size_t size() const {
			return map->size();
		}

		FieldIterator begin() {
			return {map, base, access, include_readonly};
		}

		//iterator type is not the same as begin
		std::map<std::string_view, FieldDesc>::const_iterator end() {
			return map->end();
		}

	  private:
		const void* base;
		const std::map<std::string_view, FieldDesc>* map;
		const Access access;
		const bool include_readonly;
	};

}
