#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "iinteger.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API Int : IInteger {
		Int(T* value, bool isConst)
		  : value(value), isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(value);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			value = static_cast<T*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			value = static_cast<T*>(ptr);
			isConst = false;
		}

		Var var() const override {
			return Var(value, isConst);
		}

		std::size_t size() const override {
			return sizeof(T);
		}

		bool isSigned() const override {
			return std::is_signed_v<T>;
		}

		int64_t asSigned() const override {
			return *value;
		}

		uint64_t asUnsigned() const override {
			return *value;
		}

		void setSigned(int64_t val) override {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < val || std::numeric_limits<T>::min() > val) {
				throw std::runtime_error(::astra::format("The value is too big to fit {} byte variable", sizeof(*value)));
			}

			if(val < 0 && !isSigned()) {
				throw std::runtime_error("Cannot assign negative value to unsigned");
			}

			*value = val;
			return;
		}

		void setUnsigned(uint64_t val) override {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < val || std::numeric_limits<T>::min() > val) {
				throw std::runtime_error(::astra::format("The value is too big to fit {} byte variable", sizeof(*value)));
			}

			*value = val;
			return;
		}

	  private:
		T* value;
		bool isConst;
	};

}