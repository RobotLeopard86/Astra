#pragma once

#include <functional>

#include "astra/var.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API IList {
		virtual ~IList() = default;

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
		virtual Var ownVar() const = 0;
		virtual TypeId nestedType() const = 0;
		virtual void forEach(std::function<void(Var)> callback) const = 0;
		virtual void unsafeForEach(std::function<void(void*)> callback) const = 0;
		virtual void clear() = 0;
		virtual std::size_t size() const = 0;
		virtual void push(Var value) = 0;
	};

}
