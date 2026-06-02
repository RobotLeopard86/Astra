#pragma once

#include <limits>

#include "ifloat.hpp"
#include "astra/type_query.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API Floating : IFloat {
		Floating(T* value, bool isConst)
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

		Var var() override {
			return Var(value, isConst);
		}

		std::size_t size() const override {
			return sizeof(T);
		}

		double get() const override {
			return *value;
		}

		void set(double val) override {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}
			if(val != -std::numeric_limits<double>::infinity() && val != std::numeric_limits<double>::infinity() &&
				(std::numeric_limits<T>::max() < val || -std::numeric_limits<T>::max() > val)) {
				throw std::runtime_error("The value too big to set floating-point variable");
			}

			*value = val;
			return;
		}

	  private:
		T* value;
		bool isConst;
	};

}