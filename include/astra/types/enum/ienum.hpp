#pragma once

#include "astra/var.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	class ASTRA_API IEnum {
	  public:
		virtual ~IEnum() = default;

		virtual void assign(Var var) = 0;
		virtual void unsafeAssign(void* ptr) = 0;
		virtual Var var() const = 0;
		virtual std::string toString() const = 0;
		virtual void fromString(const std::string& name) = 0;
	};

}
///@endcond
