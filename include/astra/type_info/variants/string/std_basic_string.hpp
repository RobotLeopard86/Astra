#pragma once

#include <cstddef>

#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "astra/variant/variant.hpp"
#include "istring.hpp"

namespace astra {

	template<typename T>
	struct StdBasicString : IString {
		StdBasicString() = delete;

		StdBasicString(std::basic_string<T>* str, bool isConst)
		  : _var(str, isConst) {
		}

		Expected<None> assign(Var var) override {
			if(var.type() != _var.type()) {
				return Error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),						   //
					typeName(_var.type())));
			}
			_var = var;
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_var.unsafeAssign(ptr);
		}

		std::string_view get() const override {
			return *static_cast<const std::string*>(_var.raw());
		}

		Expected<None> set(std::string_view value) override {
			if(_var.isConst()) {
				return Error("Trying to set const value");
			}
			*static_cast<std::string*>(_var.rawMut()) = std::string(value);
			return None();
		}

		Var var() const override {
			return _var;
		}

	  private:
		Var _var;
	};

}
