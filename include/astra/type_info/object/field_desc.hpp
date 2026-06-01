#pragma once

#include <cstdint>
#include <type_traits>

#include "access.hpp"
#include "astra/traits.hpp"
#include "astra/type_id.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API FieldDesc {

		template<typename T, typename ClassT>
		FieldDesc(T ClassT::* ptr, Access acc)
		  : valueData(static_cast<uintptr_t>(delta(ptr))),
			type(TypeId::get<std::remove_const_t<T>>()),
			acc(acc),
			areWeReadOnly(is_ref_type_v<T>) {
		}

		template<typename T>
		FieldDesc(T* ptr, Access acc)
		  : valueData(reinterpret_cast<uintptr_t>(ptr)),
			type(TypeId::get<std::remove_const_t<T>>()),
			acc(acc),
			areWeReadOnly(is_ref_type_v<T>) {
		}

		uintptr_t value() const {
			return valueData;
		}

		TypeId typeId() const {
			return type;
		}

		bool isConst() const {
			return (acc & Access::Const) != Access::None;
		}

		bool isStatic() const {
			return (acc & Access::Static) != Access::None;
		}

		bool isPublic() const {
			return (acc & Access::Public) != Access::None;
		}

		bool isProtected() const {
			return (acc & Access::Protected) != Access::None;
		}

		bool isPrivate() const {
			return (acc & Access::Private) != Access::None;
		}

		Access access() const {
			return acc;
		}

		bool isReadOnly() const {
			return areWeReadOnly;
		}

	  private:
		const uintptr_t valueData;//pointer for static fields, offset for members
		const TypeId type;

		const Access acc;
		const bool areWeReadOnly;

		template<class ClassT, typename T>
		std::ptrdiff_t delta(T ClassT::* ptr) {
			//NULL not nullptr_t
			return reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<ClassT const*>(NULL)->*ptr));
		}
	};
}
