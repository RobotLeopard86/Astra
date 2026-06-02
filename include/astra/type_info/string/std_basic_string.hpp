#pragma once

#include <cstddef>

#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/variant.hpp"
#include "istring.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdBasicString : IString {
		StdBasicString() = delete;

		StdBasicString(std::basic_string<T>* str, bool isConst)
		  : inner(str, isConst) {
		}

		void assign(Var var) override {
			if(var.typeId() != inner.typeId()) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(inner.typeId())));
			}
			inner = var;
			return;
		}

		void unsafeAssign(void* ptr) override {
			inner.unsafeAssign(ptr);
		}

		const std::string& get() const override {
			return *static_cast<const std::string*>(inner.raw());
		}

		void set(const std::string& value) override {
			if(inner.isConst()) {
				throw std::runtime_error("Trying to set const value");
			}
			*static_cast<std::string*>(inner.rawMut()) = value;
			return;
		}

		Var var() const override {
			return inner;
		}

	  private:
		Var inner;
	};

}
