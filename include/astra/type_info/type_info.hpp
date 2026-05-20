#pragma once

#include <string>
#include <string_view>

#include "astra/expected.hpp"
#include "astra/tools/names.hpp"
#include "astra/variant/variant.hpp"

//all variants
#include "variants/array/array.hpp"
#include "variants/bool.hpp"
#include "variants/enum/enum.hpp"
#include "variants/floating/floating.hpp"
#include "variants/integer/integer.hpp"
#include "variants/map/map.hpp"
#include "variants/object/object.hpp"
#include "variants/pointer/pointer.hpp"
#include "variants/sequence/sequence.hpp"
#include "variants/string/string.hpp"

namespace astra {

#define BASE Variant<Bool, Integer, Floating, String, Enum, Object, Array, Sequence, Map, Pointer>

	///The sum type contains information about nature of stored value
	///
	///Primitive for int, float, std::string and others
	///Object for structs and classes
	///Array
	///Sequence for arrays and containers with one generic parameter
	///Map for associative container such as std::map
	///Pointer for smart pointers
	class TypeInfo : public BASE {
	  public:
		TypeInfo(Bool value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Integer value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Floating value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(String value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Enum value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Object value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Array value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Sequence value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Map value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		TypeInfo(Pointer value)
		  : BASE(value) {//NOLINT implicit constructor
		}

		Expected<None> assign(Var var) {
			return match([=](auto&& v) { return v.assign(var); });
		}

		void unsafeAssign(void* ptr) {
			match([=](auto&& m) { m.unsafeAssign(ptr); });
		}

		Var var() {
			return match([](Array& a) -> Var { return a.ownVar(); },//
				[](Sequence& s) -> Var { return s.ownVar(); },		//
				[](Map& m) -> Var { return m.ownVar(); },			//
				[](auto&& v) { return v.var(); });
		}

		enum class Kind {
			kBool,
			kInteger,
			kFloating,
			kString,
			kEnum,
			kObject,
			kArray,
			kSequence,
			kMap,
			kPointer,
		};

		[[nodiscard]] Kind getKind() const {
			return static_cast<Kind>(index());
		}

		[[nodiscard]] std::string_view getKindStr() const {
			return match([](auto&& v) { return Names<decltype(v)>::get(); });
		}

	  private:
#ifdef _DEBUG
		Kind _kind = getKind();
#endif
	};

#undef BASE

}
