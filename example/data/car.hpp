#pragma once

#include "colors.hpp"

#include "astra/type_id.hpp"
#include "astra/types/type_actions.hpp"

#include <string>

#define REFINISH_COST 8000

class [[astra::reflect("all")]] Car {
  public:
	std::string owner, brand;
	unsigned int year;
	[[astra::alias("insured")]] bool hasInsurance = false;
	[[astra::ignore]] bool hasSatRadio = false;

	virtual int refinish(Color newColor) {
		color = newColor;
		return REFINISH_COST;
	}

	Color whatColorAmI() {
		return color;
	}

	virtual astra::TypeId ASTRA__gettypeid() const;

	virtual ~Car() {}

  private:
	Color color = Color::Green;

	friend struct astra::TypeActions<Car>;
};