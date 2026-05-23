#pragma once

#include <functional>

#include "astra/var.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API ISequence {
		virtual ~ISequence() = default;

		virtual void assign(Var var) = 0;
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
