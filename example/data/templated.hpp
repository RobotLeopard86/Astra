#pragma once

#include <cstdint>
#include <string>

#include "astra/setup.hpp"

namespace ExampleNamespace {
	template<typename P, uint8_t S>
	class ASTRA_REFLECT RandomTemplatedClass : public AstraReflectBase {
	  public:
		std::array<int, S> thingies;
		std::vector<P> otherThingies;
		std::string myKing;

		RandomTemplatedClass() = default;
		virtual ~RandomTemplatedClass();
		ASTRASETUP(RandomTemplatedClass);
	};
}