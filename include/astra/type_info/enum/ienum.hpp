#pragma once

#include "astra/var.hpp"

namespace astra {

	struct IEnum {
		virtual ~IEnum() = default;

		virtual void assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual std::string_view toString() const = 0;
		virtual void fromString(std::string_view name) = 0;
	};

}
