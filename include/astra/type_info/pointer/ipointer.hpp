#pragma once

#include "astra/var.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API IPointer {
		virtual ~IPointer() = default;

		virtual void assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual bool isNull() const = 0;
		virtual void init() = 0;
		virtual Var getNested() const = 0;
	};

}
