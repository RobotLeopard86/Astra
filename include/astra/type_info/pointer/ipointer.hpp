#pragma once

#include "astra/var.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API IPointer {
		virtual ~IPointer() = default;

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
		virtual bool isNull() const = 0;
		virtual void init() = 0;
		virtual Var getNested() const = 0;
	};

}
