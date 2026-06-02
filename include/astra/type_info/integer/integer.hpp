#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "iinteger.hpp"
#include "int.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Integer {
		Integer() = delete;

		template<typename T>
		Integer(T* value, bool isConst) {
			new(mem) Int<T>(value, isConst);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) {
			return impl()->assign(var);
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() const {
			return impl()->var();
		}

		std::size_t size() const {
			return impl()->size();
		}

		bool isSigned() const {
			return impl()->isSigned();
		}

		int64_t asSigned() const {
			return impl()->asSigned();
		}

		uint64_t asUnsigned() const {
			return impl()->asUnsigned();
		}

		template<typename T>
			requires std::is_integral_v<T> && std::is_signed_v<T>
		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(T value) {
			return impl()->setSigned(value);
		}

		template<typename T>
			requires std::is_integral_v<T> && std::is_unsigned_v<T>
		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(T value) {
			return impl()->setUnsigned(value);
		}

		void setSigned(int64_t value) {
			return impl()->setSigned(value);
		}

		void setUnsigned(uint64_t value) {
			return impl()->setUnsigned(value);
		}

	  private:
		char mem[sizeof(Int<int>)];

		inline const IInteger* impl() const {
			return reinterpret_cast<const IInteger*>(&mem[0]);
		}

		inline IInteger* impl() {
			return reinterpret_cast<IInteger*>(&mem[0]);
		}
	};

}
