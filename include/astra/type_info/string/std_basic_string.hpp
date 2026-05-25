#pragma once

#include <cstddef>

#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "astra/variant.hpp"
#include "istring.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdBasicString : IString {
		StdBasicString() = delete;

		StdBasicString(std::basic_string<T>* str, bool isConst)
		  : _var(str, isConst) {
		}

		void assign(Var var) override {
			if(var.type() != _var.type()) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.type()),												//
					typeName(_var.type())));
			}
			_var = var;
			return;
		}

		void unsafeAssign(void* ptr) override {
			_var.unsafeAssign(ptr);
		}

		const std::string& get() const override {
			return *static_cast<const std::string*>(_var.raw());
		}

		void set(const std::string& value) override {
			if(_var.isConst()) {
				throw std::runtime_error("Trying to set const value");
			}
			*static_cast<std::string*>(_var.rawMut()) = value;
			return;
		}

		Var var() const override {
			return _var;
		}

	  private:
		Var _var;
	};

}
