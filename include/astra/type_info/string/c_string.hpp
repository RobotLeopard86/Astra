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
		  : inner(str, true) {
		}

		void assign(Var var) override {
			if(var.typeId() != inner.typeId()) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.typeId()),												//
					typeName(inner.typeId())));
			}
			inner = var;
			cachedVal = static_cast<const char*>(inner.raw());
			return;
		}

		void unsafeAssign(void* ptr) override {
			inner.unsafeAssign(ptr);
			cachedVal = *static_cast<const char*>(inner.raw());
		}

		const std::string& get() const override {
			return cachedVal;
		}

		void set(const std::string&) override {
			throw std::runtime_error("Trying to set read-only value");
		}

		Var var() const override {
			return inner;
		}

	  private:
		Var inner;
		std::string cachedVal;
	};

}