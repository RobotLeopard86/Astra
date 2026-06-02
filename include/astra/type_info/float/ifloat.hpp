#pragma once

#include <iomanip>
#include <sstream>

#include "astra/dll.hpp"
#include "astra/var.hpp"

namespace astra {

	struct ASTRA_API IFloat {
		virtual ~IFloat() = default;

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		virtual void assign(Var var) = 0;
		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		virtual void unsafeAssign(void* ptr) = 0;
		virtual /**
				 * @brief Access the contents of this object as a Var
				 *
				 * @return A Var holding the contents of this object, const state inherited
				 */
			Var
			var() = 0;
		virtual std::size_t size() const = 0;
		virtual double get() const = 0;
		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		virtual void set(double value) = 0;
	};

}
