#pragma once

#include <limits>

#include "ifloat.hpp"
#include "astra/format.hpp"
#include "astra/type_query.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T>
	class ASTRA_API Floating : IFloat {
	  public:
		Floating(T* value, bool isConst)
		  : value(value), isConst(isConst) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) override {
			auto t = TypeId::get(value);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			value = static_cast<T*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) override {
			value = static_cast<T*>(ptr);
			isConst = false;
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() override {
			return Var(value, isConst);
		}

		std::size_t size() const override {
			return sizeof(T);
		}

		double get() const override {
			return *value;
		}

		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(double val) override {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}
			if(val != -std::numeric_limits<double>::infinity() && val != std::numeric_limits<double>::infinity() &&
				(std::numeric_limits<T>::max() < val || -std::numeric_limits<T>::max() > val)) {
				throw std::runtime_error("The value too big to set floating-point variable");
			}

			*value = val;
		}

	  private:
		T* value;
		bool isConst;
	};

}///@endcond
