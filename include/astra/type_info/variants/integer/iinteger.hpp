#pragma once

#include <cstddef>
#include <cstdint>

#include "astra/expected.hpp"
#include "astra/variable/var.hpp"

namespace astra {

	struct IInteger {
		virtual ~IInteger() = default;

		virtual Expected<None> assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual size_t size() const = 0;
		virtual bool isSigned() const = 0;
		virtual int64_t asSigned() const = 0;
		virtual uint64_t asUnsigned() const = 0;
		virtual Expected<None> setSigned(int64_t value) = 0;
		virtual Expected<None> setUnsigned(uint64_t value) = 0;
	};

}
