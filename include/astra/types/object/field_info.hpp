#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "access.hpp"
#include "astra/type_id.hpp"
#include "astra/dll.hpp"
#include "field_desc.hpp"
#include "astra/var.hpp"

namespace astra {
	/**
	 * @brief Representation of a concrete object field
	 */
	class ASTRA_API FieldInfo {
	  public:
		/**
		 * @brief Create a new field info from an object pointer and field description
		 *
		 * @param base Pointer to the object instance (may be @c nullptr for static fields)
		 * @param data The field description
		 */
		FieldInfo(const void* base, const FieldDesc* data)
		  : base(base), data(data) {
		}

		///@cond
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

		FieldInfo(FieldInfo&& other) {
			if(this == &other) {
				return;
			}
			base = std::exchange(other.base, nullptr);
			data = std::exchange(other.data, nullptr);
		}

		FieldInfo& operator=(FieldInfo&& other) {
			if(this == &other) {
				return *this;
			}
			base = std::exchange(other.base, nullptr);
			data = std::exchange(other.data, nullptr);
			return *this;
		}
		///@endcond

		/**
		 * @brief Access the field as a Var
		 *
		 * @return A Var holding the field, const state inherited
		 */
		Var var() const {
			if(isStatic()) {
				return {reinterpret_cast<void*>(data->value()), data->typeId(), data->isConst()};
			}
			return {shift(base, data->value()), data->typeId(), data->isConst()};
		}

		/**
		 * @brief Check if the field is const or not
		 *
		 * @return If the field is const
		 */
		bool isConst() const {
			return data->isConst();
		}

		/**
		 * @brief Check if the field is static or not
		 *
		 * @return If the field is static
		 */
		bool isStatic() const {
			return data->isStatic();
		}

		/**
		 * @brief Check if the field is public or not
		 *
		 * @return If the field is public
		 */
		bool isPublic() const {
			return data->isPublic();
		}

		/**
		 * @brief Check if the field is protected or not
		 *
		 * @return If the field is protected
		 */
		bool isProtected() const {
			return data->isProtected();
		}

		/**
		 * @brief Check if the field is private or not
		 *
		 * @return If the field is private
		 */
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