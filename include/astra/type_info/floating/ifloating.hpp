#pragma once

#include <iomanip>
#include <sstream>


#include "astra/var.hpp"

namespace astra {

	struct IFloating {
		virtual ~IFloating() = default;

		virtual void assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() = 0;
		virtual std::size_t size() = 0;
		virtual double get() const = 0;
		virtual void set(double value) = 0;
	};

}
