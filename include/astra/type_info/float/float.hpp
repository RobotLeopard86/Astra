#pragma once

#include <memory>

#include "floating.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Float final {
		Float() = delete;

		template<typename T>
		Float(T* value, bool isConst) {
			new(mem) Floating<T>(value, isConst);
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
			return impl()->unsafeAssign(ptr);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() {
			return impl()->var();
		}

		std::size_t size() const {
			return impl()->size();
		}

		double get() const {
			return impl()->get();
		}

		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(double value) {
			return impl()->set(value);
		}

	  private:
		char mem[sizeof(Floating<double>)];

		inline const IFloat* impl() const {
			return reinterpret_cast<const IFloat*>(&mem[0]);
		}

		inline IFloat* impl() {
			return reinterpret_cast<IFloat*>(&mem[0]);
		}
	};

}
