#pragma once

#include <functional>

#include "astra/variable/var.hpp"

namespace astra {

	struct IArray {
		virtual ~IArray() = default;

		virtual void assign(Var var) = 0;
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
