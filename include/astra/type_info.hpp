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
		/**
		 * @brief Enumeration of data type contained in the variant
		 */
		enum class Kind {
			Bool,	///<Boolean
			Integer,///<Signed or unsigned integer
			Float,	///<Floating point number
			String, ///<String (any kind)
			Enum,	///<Enumeration value
			Object, ///<Class or struct
			Array,	///<Fixed-size container
			List,	///<Resizable container
			Map,	///<Key-value store
			Pointer,///<Smart pointer
		};

		/**
		 * @brief Create a new type info variant storing a Bool
		 *
		 * @param value The value to store
		 */
		TypeInfo(Bool value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing a Integer
		 *
		 * @param value The value to store
		 */
		TypeInfo(Integer value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing a Float
		 *
		 * @param value The value to store
		 */
		TypeInfo(Float value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing a String
		 *
		 * @param value The value to store
		 */
		TypeInfo(String value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing an Enum
		 *
		 * @param value The value to store
		 */
		TypeInfo(Enum value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing an Object
		 *
		 * @param value The value to store
		 */
		TypeInfo(Object value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing an Array
		 *
		 * @param value The value to store
		 */
		TypeInfo(Array value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing a List
		 *
		 * @param value The value to store
		 */
		TypeInfo(List value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing a Map
		 *
		 * @param value The value to store
		 */
		TypeInfo(Map value)
		  : BASE(value) {
		}

		/**
		 * @brief Create a new type info variant storing a Pointer
		 *
		 * @param value The value to store
		 */
		TypeInfo(Pointer value)
		  : BASE(value) {
		}

		/**
		 * @brief Replace the current value in the variant with a new one
		 *
		 * @param var The new value to store
		 */
		void assign(Var var) {
			match([=](auto&& v) { return v.assign(var); });
		}

		/**
		 * @brief Unsafely replace the current value in the variant
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr A pointer to the new object to store
		 */
		void unsafeAssign(void* ptr) {
			match([=](auto&& v) { return v.unsafeAssign(ptr); });
		}

		/**
		 * @brief Get the contents of the type info variant as a Var
		 *
		 * @return Current value as Var
		 */
		Var var() {
			return match([](Array& a) -> Var { return a.ownVar(); },
				[](List& s) -> Var { return s.ownVar(); },
				[](Map& m) -> Var { return m.ownVar(); },
				[](auto&& v) { return v.var(); });
		}

		/**
		 * @brief Get what kind of value is stored in the variant
		 *
		 * @return Type of value currently stored
		 */
		[[nodiscard]] Kind getKind() const {
			return static_cast<Kind>(index());
		}

		/**
		 * @brief Get the name of the value kind currently stored in the variant
		 *
		 * @return Name of the currently-stored type
		 */
		[[nodiscard]] std::string getKindStr() const {
			return match([](auto&& v) { return Names<decltype(v)>::get(); });
		}
	};

#undef BASE

}
