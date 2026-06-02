#pragma once

#include <string>

#include "astra/names.hpp"
#include "astra/variant.hpp"
#include "astra/dll.hpp"
#include "types/array/array.hpp"
#include "types/bool.hpp"
#include "types/enum/enum.hpp"
#include "types/float/float.hpp"
#include "types/integer/integer.hpp"
#include "types/map/map.hpp"
#include "types/object/object.hpp"
#include "types/pointer/pointer.hpp"
#include "types/list/list.hpp"
#include "types/string/string.hpp"

namespace astra {

#define BASE Variant<Bool, Integer, Float, String, Enum, Object, Array, List, Map, Pointer>

	/**
	 * @brief Sum-type variant to hold various type info classes
	 */
	class ASTRA_API TypeInfo : public BASE {
	  public:
		TypeInfo(Bool value)
		  : BASE(value) {
		}

		TypeInfo(Integer value)
		  : BASE(value) {
		}

		TypeInfo(Float value)
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
			match([=](auto&& v) { return v.assign(var); });
		}

		void unsafeAssign(void* ptr) {
			match([=](auto&& v) { return v.unsafeAssign(ptr); });
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
			Float,
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
