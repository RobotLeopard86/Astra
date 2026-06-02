#pragma once

#include <cstddef>
#include <cstdint>

#include "astra/dll.hpp"
#include "astra/var.hpp"

namespace astra {

	struct ASTRA_API IInteger {
		virtual ~IInteger() = default;

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
		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		virtual Var var() const = 0;
		virtual std::size_t size() const = 0;
		virtual bool isSigned() const = 0;
		virtual int64_t asSigned() const = 0;
		virtual uint64_t asUnsigned() const = 0;
		virtual void setSigned(int64_t value) = 0;
		virtual void setUnsigned(uint64_t value) = 0;
	};

}
