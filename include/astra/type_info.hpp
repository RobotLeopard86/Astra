#pragma once

#include <string>
#include <string_view>


#include "astra/names.hpp"
#include "astra/variant.hpp"
#include "astra/dll.hpp"

//all variants
#include "type_info/array/array.hpp"
#include "type_info/bool.hpp"
#include "type_info/enum/enum.hpp"
#include "type_info/floating/floating.hpp"
#include "type_info/integer/integer.hpp"
#include "type_info/map/map.hpp"
#include "type_info/object/object.hpp"
#include "type_info/pointer/pointer.hpp"
#include "type_info/list/list.hpp"
#include "type_info/string/string.hpp"

namespace astra {

#define BASE Variant<Bool, Integer, Floating, String, Enum, Object, Array, List, Map, Pointer>

	///The sum type contains information about nature of stored value
	///
	///Primitive for int, float, std::string and others
	///Object for structs and classes
	///Array
	///List for arrays and containers with one generic parameter
	///Map for associative container such as std::map
	///Pointer for smart pointers
	class ASTRA_API TypeInfo : public BASE {
	  public:
		TypeInfo(Bool value)
		  : BASE(value) {
		}

		TypeInfo(Integer value)
		  : BASE(value) {
		}

		TypeInfo(Floating value)
		  : BASE(value) {
		}

		TypeInfo(String value)
		  : BASE(value) {
		}

		TypeInfo(Enum value)
		  : BASE(value) {
		}

		TypeInfo(Object value)
		  : BASE(value) {
		}

		TypeInfo(Array value)
		  : BASE(value) {
		}

		TypeInfo(List value)
		  : BASE(value) {
		}

		TypeInfo(Map value)
		  : BASE(value) {
		}

		TypeInfo(Pointer value)
		  : BASE(value) {
		}

		void assign(Var var) {
			return match([=](auto&& v) { return v.assign(var); });
		}

		void unsafeAssign(void* ptr) {
			match([=](auto&& m) { m.unsafeAssign(ptr); });
		}

		Var var() {
			return match([](Array& a) -> Var { return a.ownVar(); },
				[](List& s) -> Var { return s.ownVar(); },
				[](Map& m) -> Var { return m.ownVar(); },
				[](auto&& v) { return v.var(); });
		}

		enum class Kind {
			Bool,
			Integer,
			Floating,
			String,
			Enum,
			Object,
			Array,
			List,
			Map,
			Pointer,
		};

		[[nodiscard]] Kind getKind() const {
			return static_cast<Kind>(index());
		}

		[[nodiscard]] std::string getKindStr() const {
			return match([](auto&& v) { return Names<decltype(v)>::get(); });
		}
	};

#undef BASE

}
