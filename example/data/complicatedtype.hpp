#pragma once

#include "car.hpp"
#include "colors.hpp"

#include "astra/setup.hpp"

#include <string>

namespace ExampleNamespace {
	class ASTRA_REFLECT ComplicatedType : public AstraReflectBase {
	  public:
		std::string myString;
		std::map<Color, std::array<uint16_t, 5>> someMap;

		enum class ASTRA_REFLECT AnotherEnum {
			Mantequilla,
			BigChungus,
			SeisSiete,
			ArrozConPollo
		};

		std::unique_ptr<AnotherEnum> ptrToAnotherEnum;
		std::unique_ptr<std::shared_ptr<Car>> carrrrr;

		virtual ~ComplicatedType() {}

		ASTRASETUP(ComplicatedType)
	};
}