#pragma once

#include <limits>

#include "astra/type_name.hpp"
#include "ifloating.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API Float : IFloating {
		Float(T* value, bool isConst)
		  : _value(value), _isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_value);
			if(var.type() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.type()),												//
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

		Var var() override {
			return Var(_value, _isConst);
		}

		std::size_t size() const override {
			return sizeof(T);
		}

		double get() const override {
			return *_value;
		}

		void set(double value) override {
			if(_isConst) {
				throw std::runtime_error("Trying to set const value");
			}
			if(value != -std::numeric_limits<double>::infinity() && value != std::numeric_limits<double>::infinity() &&
				(std::numeric_limits<T>::max() < value || -std::numeric_limits<T>::max() > value)) {
				throw std::runtime_error("The value too big to set float variable");
			}

			*_value = value;
			return;
		}

	  private:
		T* _value;
		bool _isConst;
	};

}