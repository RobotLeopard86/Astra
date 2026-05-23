#pragma once

#include <cstddef>

#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "astra/variant.hpp"
#include "istring.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API CString : IString {

		CString(const T** str)
		  : _var(str, true) {
		}

		void assign(Var var) override {
			if(var.type() != _var.type()) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(_var.type())));
			}
			_var = var;
			return;
		}

		void unsafeAssign(void* ptr) override {
			_var.unsafeAssign(ptr);
		}

		std::string_view get() const override {
			return *static_cast<T* const*>(_var.raw());
		}

		void set(std::string_view value) override {
			throw std::runtime_error("Trying to set const value");
			//keep it as possible implementation
			//if (_var.isConst()) {
			//  throw std::runtime_error("Trying to set const value");
			//}
			//if (*value.end() != '\0') {
			//  throw std::runtime_error("C string value should be null terminated");
			//}
			//*static_cast<const T**>(_var.rawMut()) = value.data();
			//return;
		}

		Var var() const override {
			return _var;
		}

	  private:
		Var _var;
	};

}