#pragma once

#include <cstddef>
#include <cstdint>


#include "astra/var.hpp"

namespace astra {

	struct IInteger {
		virtual ~IInteger() = default;

		virtual void assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual std::size_t size() const = 0;
		virtual bool isSigned() const = 0;
		virtual int64_t asSigned() const = 0;
		virtual uint64_t asUnsigned() const = 0;
		virtual void setSigned(int64_t value) = 0;
		virtual void setUnsigned(uint64_t value) = 0;
	};

}
