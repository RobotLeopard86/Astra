#pragma once

#include "colors.hpp"
#include "car.hpp"

#include "astra/setup.hpp"

#define SUV_REFINISH_FEE 1500

namespace ExampleNamespace {
	class ASTRA_REFLECT SUV : public Car {
	  public:
		std::string nickname;
		float trunkVolume;

		virtual int refinish(Color newColor) override {
			return Car::refinish(newColor) + SUV_REFINISH_FEE;
		}

		ASTRASETUP(SUV)
	};
}