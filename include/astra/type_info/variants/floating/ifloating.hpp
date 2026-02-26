#pragma once

#include <iomanip>
#include <sstream>

#include "astra/expected.hpp"
#include "astra/variable/var.hpp"

namespace astra {

	struct IFloating {
		virtual ~IFloating() = default;

		virtual Expected<None> assign(Var var) = 0;
		virtual void unsafe_assign(void* ptr) = 0;
		virtual Var var() = 0;
		virtual size_t size() = 0;
		virtual double get() const = 0;
		virtual Expected<None> set(double value) = 0;
	};

}
