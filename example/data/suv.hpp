#pragma once

#include "colors.hpp"
#include "car.hpp"

#define SUV_REFINISH_FEE 1500

namespace ExampleNamespace {
	class [[astra::reflect("all")]] SUV : public Car {
	  public:
		std::string nickname;
		float trunkVolume;

		virtual int refinish(Color newColor) override {
			return Car::refinish(newColor) + SUV_REFINISH_FEE;
		}

		virtual astra::TypeId ASTRA__gettypeid() const override;
	};
}