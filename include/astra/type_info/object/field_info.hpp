#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "access.hpp"
#include "astra/type_id.hpp"
#include "astra/dll.hpp"
#include "field_desc.hpp"
#include "astra/var.hpp"

namespace astra {

	class ASTRA_API FieldInfo {
	  public:
		FieldInfo(const void* base, const FieldDesc* data)
		  : base(base), data(data) {
		}

		FieldInfo(const FieldInfo& other) {
			if(this == &other) {
				return;
			}
			base = other.base;
			data = other.data;
		}

		FieldInfo& operator=(const FieldInfo& other) {
			if(this == &other) {
				return *this;
			}
			base = other.base;
			data = other.data;
			return *this;
		}

		Var var() const {
			if(isStatic()) {
				return {reinterpret_cast<void*>(data->value()), data->typeId(), data->isConst()};
			}
			return {shift(base, data->value()), data->typeId(), data->isConst()};
		}

		bool isConst() const {
			return data->isConst();
		}

		bool isStatic() const {
			return data->isStatic();
		}

		bool isPublic() const {
			return data->isPublic();
		}

		bool isProtected() const {
			return data->isProtected();
		}

		bool isPrivate() const {
			return data->isPrivate();
		}

	  private:
		const void* base;
		const FieldDesc* data;

		static void* shift(const void* base, uintptr_t offset) {
			return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + offset);
		}
	};

}
