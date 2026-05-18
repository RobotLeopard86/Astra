#pragma once

#include <cstddef>

#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "astra/variant/variant.hpp"
#include "istring.hpp"

namespace astra {

	template<typename T>
	struct CString : IString {

		CString(const T** str)
		  : _var(str, true) {
		}

		Expected<None> assign(Var var) override {
			if(var.type() != _var.type()) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::typeName(var.type()),					  //
					reflection::typeName(_var.type())));
			}
			_var = var;
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_var.unsafeAssign(ptr);
		}

		std::string_view get() const override {
			return *static_cast<T* const*>(_var.raw());
		}

		Expected<None> set(std::string_view value) override {
			return Error("Trying to set const value");
			//keep it as possible implementation
			//if (_var.isConst()) {
			//  return Error("Trying to set const value");
			//}
			//if (*value.end() != '\0') {
			//  return Error("C string value should be null terminated");
			//}
			//*static_cast<const T**>(_var.rawMut()) = value.data();
			//return None();
		}

		Var var() const override {
			return _var;
		}

	  private:
		Var _var;
	};

}