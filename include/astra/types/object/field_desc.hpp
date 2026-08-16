#pragma once

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
		 * @brief Create a field description for a member
		 *
		 * @tparam T The type of the field
		 * @tparam C The type of the containing class
		 *
		 * @param ptr A pointer to the member, like so: @c &SomeClass::someField
		 * @param acc The field's access rules
		 */
		template<typename T, typename C>
		FieldDesc(T C::* ptr, Access acc)
		  : resolve([ptr](void* base) {
				return std::addressof(static_cast<C*>(base)->*ptr);
			}),
			type(TypeId::get<std::remove_const_t<T>>()),
			acc(acc),
			areWeReadOnly(is_ref_type_v<T>) {
		}

		/**
		 * @brief Create a field description for a static field
		 *
		 * @tparam T The type of the field
		 *
		 * @param ptr A pointer to the field
		 * @param acc The field's access rules
		 */
		template<typename T>
		FieldDesc(T* ptr, Access acc)
		  : resolve([ptr](void*) { return ptr; }),
			type(TypeId::get<std::remove_const_t<T>>()),
			acc(acc),
			areWeReadOnly(is_ref_type_v<T>) {
		}

		const std::function<void*(void*)> resolve;///<Returns a pointer to the field (parameter ignored for static fields, parameter is base for members)

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
		const TypeId type;

		const Access acc;
		const bool areWeReadOnly;
	};
}
