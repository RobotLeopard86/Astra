#pragma once

#include <functional>

#include "astra/var.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	class ASTRA_API IArray {
	  public:
		virtual ~IArray() = default;

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
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void unsafeForEach(std::function<void(void*)> callback) const = 0;
		virtual std::size_t size() const = 0;
		virtual Var front() = 0;
		virtual Var back() = 0;
		virtual Var at(std::size_t idx) = 0;
		virtual Var operator[](std::size_t idx) = 0;
		virtual void fill(Var filler) = 0;
	};

}
///@endcond
