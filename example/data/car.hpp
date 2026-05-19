#pragma once

#include "colors.hpp"

#include "astra/reflectable.hpp"

#include <string>

#define REFINISH_COST 8000

class ASTRA_REFLECT Car : public AstraReflectBase {
  public:
	std::string owner, brand;
	unsigned int year;
	ASTRA_ALIAS("insured")
	bool hasInsurance = false;
	ASTRA_IGNORE bool hasSatRadio = false;

	virtual int refinish(Color newColor) {
		color = newColor;
		return REFINISH_COST;
	}

	Color whatColorAmI() {
		return color;
	}

	virtual ~Car() {}

	ASTRASETUP(Car)

  private:
	Color color = Color::Green;
};