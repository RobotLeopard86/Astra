#pragma once

#include "astra/variable/var.hpp"

namespace astra {

	struct IPointer {
		virtual ~IPointer() = default;

		virtual Expected<None> assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual bool isNull() const = 0;
		virtual void init() = 0;
		virtual Expected<Var> getNested() const = 0;
	};

}
