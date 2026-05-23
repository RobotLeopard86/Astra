#pragma once

#include <cstddef>

#include "field_iterator.hpp"

#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Fields {
		Fields(const void* base, const std::map<std::string_view, FieldDesc>* map, Access access, bool includeReadonly)//
		  : _base(base), _map(map), _access(access), _include_readonly(includeReadonly) {
		}

		std::size_t size() const {
			return _map->size();
		}

		FieldIterator begin() {
			return {_map, _base, _access, _include_readonly};
		}

		//iterator type is not the same as begin
		std::map<std::string_view, FieldDesc>::const_iterator end() {
			return _map->end();
		}

	  private:
		const void* _base;
		const std::map<std::string_view, FieldDesc>* _map;
		const Access _access;
		const bool _include_readonly;
	};

}
