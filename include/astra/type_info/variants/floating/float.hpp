#pragma once

#include <limits>

#include "astra/reflection/type_name.hpp"
#include "ifloating.hpp"

namespace astra {

	template<typename T>
	struct Float : IFloating {
		Float(T* value, bool isConst)
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

		Var var() override {
			return Var(_value, _isConst);
		}

		size_t size() override {
			return sizeof(T);
		}

		double get() const override {
			return *_value;
		}

		Expected<None> set(double value) override {
			if(_isConst) {
				return Error("Trying to set const value");
			}
			if(value != -std::numeric_limits<double>::infinity() && value != std::numeric_limits<double>::infinity() &&
				(std::numeric_limits<T>::max() < value || -std::numeric_limits<T>::max() > value)) {
				return Error("The value too big to set float variable");
			}

			*_value = value;
			return None();
		}

	  private:
		T* _value;
		bool _isConst;
	};

}