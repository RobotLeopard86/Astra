#pragma once

#include <string_view>

#include "astra/expected.hpp"
#include "astra/variable/var.hpp"

namespace astra {

	struct IString {
		virtual ~IString() = default;

		virtual Expected<None> assign(Var var) = 0;
		virtual void unsafe_assign(void* ptr) = 0;
		virtual std::string_view get() const = 0;
		virtual Expected<None> set(std::string_view value) = 0;
		virtual Var var() const = 0;
	};

}
