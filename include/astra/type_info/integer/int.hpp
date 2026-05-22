#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "iinteger.hpp"

namespace astra {

	template<typename T>
	struct Int : IInteger {
		Int(T* value, bool isConst)
		  : _value(value), _isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_value);
			if(var.type() != t) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(t)));
			}

			_value = static_cast<T*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_value = static_cast<T*>(ptr);
			_isConst = false;
		}

		Var var() const override {
			return Var(_value, _isConst);
		}

		std::size_t size() const override {
			return sizeof(T);
		}

		bool isSigned() const override {
			return std::is_signed_v<T>;
		}

		int64_t asSigned() const override {
			return *_value;
		}

		uint64_t asUnsigned() const override {
			return *_value;
		}

		void setSigned(int64_t value) override {
			if(_isConst) {
				throw std::runtime_error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < value || std::numeric_limits<T>::min() > value) {
				throw std::runtime_error(format("The value is too big to fit {} byte variable", sizeof(*_value)));
			}

			if(value < 0 && !isSigned()) {
				throw std::runtime_error("Cannot assign negative value to unsigned");
			}

			*_value = value;
			return;
		}

		void setUnsigned(uint64_t value) override {
			if(_isConst) {
				throw std::runtime_error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < value || std::numeric_limits<T>::min() > value) {
				throw std::runtime_error(format("The value is too big to fit {} byte variable", sizeof(*_value)));
			}

			*_value = value;
			return;
		}

	  private:
		T* _value;
		bool _isConst;
	};

}