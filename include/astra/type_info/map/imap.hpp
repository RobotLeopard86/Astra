#pragma once

#include <functional>

#include "astra/var.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	class ASTRA_API IMap {
	  public:
		virtual ~IMap() = default;

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
		virtual TypeId keyType() const = 0;
		virtual TypeId valType() const = 0;
		virtual void forEach(std::function<void(Var, Var)> callback) const = 0;
		virtual void forEach(std::function<void(Var, Var)> callback) = 0;
		virtual void unsafeForEach(std::function<void(void*, void*)> callback) const = 0;
		virtual void clear() = 0;
		virtual std::size_t size() const = 0;
		virtual void insert(Var key, Var value) = 0;
		virtual void remove(Var key) = 0;
	};

}
///@endcond
