#pragma once

#include "astra/setup.hpp"

#include <cmath>
#include <string>
#include <format>

class SubstitutedType {
  public:
	std::string someData;
	int computed;

	struct ASTRA_REFLECT Serialized : public AstraReflectBase {
		int base;
		ASTRASETUP(Serialized)
		virtual ~Serialized() {}
	};

	SubstitutedType() {}
	SubstitutedType(Serialized s) {
		someData = std::format("I have {} dogs", s.base);
		computed = std::pow(2, s.base);
	}

	SubstitutedType(const SubstitutedType&) = default;
	SubstitutedType(SubstitutedType&&) = default;
	SubstitutedType& operator=(const SubstitutedType&) = default;
	SubstitutedType& operator=(SubstitutedType&&) = default;

	Serialized ASTRA__getserialized() const noexcept {
		Serialized s = {};
		s.base = std::ceil(std::log2(computed));
		return s;
	}
};