#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "iinteger.hpp"

namespace astra {

	template<typename T>
	struct Int : IInteger {
		Int(T* value, bool isConst)
		  : _value(value), _isConst(isConst) {
		}

		Expected<None> assign(Var var) override {
			auto t = TypeId::get(_value);
			if(var.type() != t) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::typeName(var.type()),					  //
					reflection::typeName(t)));
			}

			_value = static_cast<T*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return None();
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

		Expected<None> setSigned(int64_t value) override {
			if(_isConst) {
				return Error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < value || std::numeric_limits<T>::min() > value) {
				return Error(astra::format("The value is too big to fit {} byte variable", sizeof(*_value)));
			}

			if(value < 0 && !isSigned()) {
				return Error("Cannot assign negative value to unsigned");
			}

			*_value = value;
			return None();
		}

		Expected<None> setUnsigned(uint64_t value) override {
			if(_isConst) {
				return Error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < value || std::numeric_limits<T>::min() > value) {
				return Error(astra::format("The value is too big to fit {} byte variable", sizeof(*_value)));
			}

			*_value = value;
			return None();
		}

	  private:
		T* _value;
		bool _isConst;
	};

}