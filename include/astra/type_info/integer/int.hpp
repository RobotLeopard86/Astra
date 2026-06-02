#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "iinteger.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API Int : IInteger {
		Int(T* value, bool isConst)
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
		}

		void setUnsigned(uint64_t val) override {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}

			if(std::numeric_limits<T>::max() < val || std::numeric_limits<T>::min() > val) {
				throw std::runtime_error(::astra::format("The value is too big to fit {} byte variable", sizeof(*value)));
			}

			*value = val;
		}

	  private:
		T* value;
		bool isConst;
	};

}