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
			return *static_cast<T* const*>(_var.raw());
		}

		void set(const std::string&) override {
			throw std::runtime_error("Trying to set const value");
		}

		Var var() const override {
			return _var;
		}

	  private:
		Var _var;
	};

}