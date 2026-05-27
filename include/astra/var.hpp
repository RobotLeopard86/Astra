#pragma once

#include "dll.hpp"
#include "reflectable.hpp"
#include "type_id.hpp"

namespace astra {

	///The abstraction from type and const modifier
	///all types represented like a pointer + type id + const flag
	///the same representation for each type
	struct ASTRA_API Var {
		Var();
		Var(void* value, TypeId type, bool isConst);

		template<typename T>
		explicit Var(const T* value)
		  : _value(const_cast<T*>(value)), _type(TypeId::get<T>()), _isConst(true) {
		}

		template<typename T>
		explicit Var(T* value, bool isConst = false)
		  : _value(value), _type(TypeId::get(value)), _isConst(isConst) {
		}

		template<Reflectable T>
			requires(!std::is_enum_v<T> && std::is_class_v<T>)
		explicit Var(const T* value)
		  : _value(const_cast<T*>(value)), _type(value != nullptr ? value->ASTRA__gettypeid() : TypeId::get<T>()), _isConst(true) {}

		template<Reflectable T>
			requires(!std::is_enum_v<T> && std::is_class_v<T>)
		explicit Var(T* value, bool isConst = false)
		  : _value(value), _type(value != nullptr ? value->ASTRA__gettypeid() : TypeId::get<T>()), _isConst(isConst) {}

		void unsafeAssign(void* ptr);

		bool operator==(const Var& other) const;
		bool operator!=(const Var& other) const;

		void* rawMut() const;
		const void* raw() const;

		TypeId type() const;

		bool isConst() const;

		void dispose();

		///runtime type check and cast
		template<typename T>
		T* rtCast() const {
			if(std::is_const_v<T> == false && isConst()) {
				throw std::runtime_error("The type under Var has const qualifier, cannot cast to mutable");
			}

			auto desiredType = TypeId::get<std::remove_const_t<T>>();

			if(desiredType != _type) {
				error(_type, desiredType);
			}

			return static_cast<T*>(_value);
		}

	  private:
		void* _value;
		TypeId _type;
		bool _isConst;

		//include reflection header into .cpp file to avoid cyclic dependencies
		static void error(TypeId type, TypeId desiredType);
	};

}
