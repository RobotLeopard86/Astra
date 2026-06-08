#pragma once

#include <string_view>

#include "astra/dll.hpp"
#include "astra/var.hpp"

///@cond
namespace astra {

	class ASTRA_API IString {
	  public:
		virtual ~IString() = default;

		virtual void assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual const std::string& get() const = 0;
		virtual void set(const std::string& value) = 0;
		virtual Var var() const = 0;
	};

}
///@endcond
