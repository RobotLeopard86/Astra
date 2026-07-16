#pragma once

#include "astra/serialized_substitute.hpp"
#include "astra/setup.hpp"

#include <cmath>
#include <string>
#include <format>

class SubstitutedType;

template<>
struct ASTRA_REFLECT astra::SerializedSubstitute<SubstitutedType> : public AstraReflectBase {
	int base;
	ASTRASETUP(SerializedSubstitute)
	virtual ~SerializedSubstitute() {}
};

class SubstitutedType {
  public:
	std::string someData;
	int computed;

	SubstitutedType() {}
	SubstitutedType(astra::SerializedSubstitute<SubstitutedType> s) {
		someData = std::format("I have {} dogs", s.base);
		computed = std::pow(2, s.base);
	}

	SubstitutedType(const SubstitutedType&) = default;
	SubstitutedType(SubstitutedType&&) = default;
	SubstitutedType& operator=(const SubstitutedType&) = default;
	SubstitutedType& operator=(SubstitutedType&&) = default;

	astra::SerializedSubstitute<SubstitutedType> ASTRA__getserialized() const noexcept {
		astra::SerializedSubstitute<SubstitutedType> s = {};
		s.base = std::ceil(std::log2(computed));
		return s;
	}
};