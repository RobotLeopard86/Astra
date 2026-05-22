#pragma once

#include <functional>

#include "astra/var.hpp"

namespace astra {

	struct IMap {
		virtual ~IMap() = default;

		virtual void assign(Var var) = 0;
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
