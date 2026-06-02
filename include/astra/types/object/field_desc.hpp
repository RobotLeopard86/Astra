#pragma once

#include <cstdint>
#include <type_traits>

#include "access.hpp"
#include "astra/traits.hpp"
#include "astra/type_id.hpp"
#include "astra/dll.hpp"

namespace astra {
	/**
	 * @brief Description of a member field in an object
	 */
	class ASTRA_API FieldDesc {
	  public:
		/**
		 * @brief Create a new field description
		 *
		 * @tparam T The type of the field
		 * @tparam C The type of the containing class
		 *
		 * @param ptr A pointer to the field
		 * @param acc The field's access rules
		 */
		template<typename T, typename C>
		FieldDesc(T C::* ptr, Access acc)
		  : valueData(static_cast<uintptr_t>(delta(ptr))),
			type(TypeId::get<std::remove_const_t<T>>()),
			acc(acc),
			areWeReadOnly(is_ref_type_v<T>) {
		}

		/**
		 * @brief Create a new field description
		 *
		 * @tparam T The type of the field
		 *
		 * @param ptr A pointer to the field
		 * @param acc The field's access rules
		 */
		template<typename T>
		FieldDesc(T* ptr, Access acc)
		  : valueData(reinterpret_cast<uintptr_t>(ptr)),
			type(TypeId::get<std::remove_const_t<T>>()),
			acc(acc),
			areWeReadOnly(is_ref_type_v<T>) {
		}

		/**
		 * @brief Get the field value
		 *
		 * @return A pointer to the field if it's static or an offset into the class if it's not
		 */
		uintptr_t value() const {
			return valueData;
		}

		/**
		 * @brief Get the type ID of the field
		 *
		 * @return The field's type ID
		 */
		TypeId typeId() const {
			return type;
		}

		/**
		 * @brief Get the field's access bitmask
		 *
		 * @return The field's access
		 */
		Access access() const {
			return acc;
		}

		/**
		 * @brief Check if the field is const or not
		 *
		 * @return If the field is const
		 */
		bool isConst() const {
			return (acc & Access::Const) != Access::None;
		}

		/**
		 * @brief Check if the field is static or not
		 *
		 * @return If the field is static
		 */
		bool isStatic() const {
			return (acc & Access::Static) != Access::None;
		}

		/**
		 * @brief Check if the field is public or not
		 *
		 * @return If the field is public
		 */
		bool isPublic() const {
			return (acc & Access::Public) != Access::None;
		}

		/**
		 * @brief Check if the field is protected or not
		 *
		 * @return If the field is protected
		 */
		bool isProtected() const {
			return (acc & Access::Protected) != Access::None;
		}

		/**
		 * @brief Check if the field is private or not
		 *
		 * @return If the field is private
		 */
		bool isPrivate() const {
			return (acc & Access::Private) != Access::None;
		}

		/**
		 * @brief Check if the field is read-only or not
		 *
		 * @return If the field is read-only
		 */
		bool isReadOnly() const {
			return areWeReadOnly;
		}

	  private:
		const uintptr_t valueData;///<Pointer for static fields, offset for members
		const TypeId type;

		const Access acc;
		const bool areWeReadOnly;

		template<class C, typename T>
		std::ptrdiff_t delta(T C::* ptr) {
			//Using NULL intentionally, DO NOT sub for nullptr
			return reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<C const*>(NULL)->*ptr));
		}
	};
}
