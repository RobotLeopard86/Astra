#pragma once

#include "astra/variable/var.hpp"

namespace astra {

	struct IEnum {
		virtual ~IEnum() = default;

		virtual Expected<None> assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual std::string_view toString() const = 0;
		virtual Expected<None> parse(std::string_view name) = 0;
	};

}
