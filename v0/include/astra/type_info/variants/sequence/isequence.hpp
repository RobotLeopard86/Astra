#pragma once

#include <functional>

#include "astra/variable/var.hpp"

namespace astra {

	struct ISequence {
		virtual ~ISequence() = default;

		virtual Expected<None> assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var ownVar() const = 0;
		virtual TypeId nestedType() const = 0;
		virtual void forEach(std::function<void(Var)> callback) const = 0;
		virtual void unsafeForEach(std::function<void(void*)> callback) const = 0;
		virtual void clear() = 0;
		virtual std::size_t size() const = 0;
		virtual Expected<None> push(Var value) = 0;
	};

}
