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
		  : _value(static_cast<uintptr_t>(delta(ptr))),
			_type(TypeId::get<std::remove_const_t<T>>()),
			_acc(acc),
			_is_readonly(is_ref_type_v<T>) {
		}

		template<typename T>
		FieldDesc(T* ptr, Access acc)
		  : _value(reinterpret_cast<uintptr_t>(ptr)),
			_type(TypeId::get<std::remove_const_t<T>>()),
			_acc(acc),
			_is_readonly(is_ref_type_v<T>) {
		}

		uintptr_t value() const {
			return _value;
		}

		TypeId type() const {
			return _type;
		}

		bool isConst() const {
			return (_acc & Access::Const) != Access::None;
		}

		bool isStatic() const {
			return (_acc & Access::Static) != Access::None;
		}

		bool isPublic() const {
			return (_acc & Access::Public) != Access::None;
		}

		bool isProtected() const {
			return (_acc & Access::Protected) != Access::None;
		}

		bool isPrivate() const {
			return (_acc & Access::Private) != Access::None;
		}

		Access access() const {
			return _acc;
		}

		bool isReadonly() const {
			return _is_readonly;
		}

	  private:
		const uintptr_t _value;//pointer for static fields, offset for members
		const TypeId _type;

		const Access _acc;
		const bool _is_readonly;

		template<class ClassT, typename T>
		std::ptrdiff_t delta(T ClassT::* ptr) {
			//NULL not nullptr_t
			return reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<ClassT const*>(NULL)->*ptr));
		}
	};
}
