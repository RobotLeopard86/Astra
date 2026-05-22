#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "access.hpp"
#include "astra/type_id.hpp"
#include "field_desc.hpp"

namespace astra {

	class FieldInfo {
	  public:
		FieldInfo(const void* base, const FieldDesc* data)
		  : _base(base), _data(data) {
		}

		FieldInfo(const FieldInfo& other) {
			if(this == &other) {
				return;
			}
			_base = other._base;
			_data = other._data;
		}

		FieldInfo& operator=(const FieldInfo& other) {
			if(this == &other) {
				return *this;
			}
			_base = other._base;
			_data = other._data;
			return *this;
		}

		Var var() const {
			if(isStatic()) {
				return {reinterpret_cast<void*>(_data->value()), _data->type(), _data->isConst()};
			}
			return {shift(_base, _data->value()), _data->type(), _data->isConst()};
		}

		bool isConst() const {
			return _data->isConst();
		}

		bool isStatic() const {
			return _data->isStatic();
		}

		bool isPublic() const {
			return _data->isPublic();
		}

		bool isProtected() const {
			return _data->isProtected();
		}

		bool isPrivate() const {
			return _data->isPrivate();
		}

	  private:
		const void* _base;
		const FieldDesc* _data;

		static void* shift(const void* base, uintptr_t offset) {
			return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + offset);
		}
	};

}
