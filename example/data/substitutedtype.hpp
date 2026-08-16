#pragma once

#include "astra/reflectable.hpp"
#include "astra/serialized_substitute.hpp"
#include "astra/setup.hpp"

#include <cmath>
#include <string>
#include <format>

class SubstitutedType {
  public:
	std::string someData;
	int computed;

	SubstitutedType() = default;
	SubstitutedType(const SubstitutedType&) = default;
	SubstitutedType(SubstitutedType&&) = default;
	SubstitutedType& operator=(const SubstitutedType&) = default;
	SubstitutedType& operator=(SubstitutedType&&) = default;
};

template<>
struct ASTRA_REFLECT astra::SerializedSubstitute<SubstitutedType> : public AstraReflectBase {
	int base;

	SerializedSubstitute() = default;

	SerializedSubstitute(const SubstitutedType& st) {
		base = std::log2(st.computed);
	}

	ASTRA_SUBSTITUTE_SERIALIZE(SubstitutedType) {
		base = std::log2(in->computed);
	}

	ASTRA_SUBSTITUTE_DESERIALIZE(SubstitutedType) {
		out->computed = std::pow(2, base);
		out->someData = std::format("I have {} dogs", base);
	}

	ASTRASETUP(SerializedSubstitute)
	virtual ~SerializedSubstitute() {}
};