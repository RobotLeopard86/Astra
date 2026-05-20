#include "data/car.hpp"
#include "data/colors.hpp"
#include "data/suv.hpp"

#include "example.astra.hpp"

#include "astra/reflection/reflection.hpp"
#include "astra/serialization/json.hpp"
#include "astra/serialization/convert.hpp"

#include <iostream>

int main() {
	//We have a car
	Car car = {};
	car.brand = "Toyota";
	car.year = 2023;
	car.owner = "John Smith";

	//Let's dump it to the console as JSON
	std::string asJson = astra::json::toString(&car).unwrap();
	std::cout << "I have a car:\n"
			  << asJson << std::endl;

	//Let's turn the JSON into YAML
	std::cout << "This can also be in YAML:\n---\n"
			  << astra::converters::toYaml::fromJsonString<Car>(asJson).unwrap() << "\n---" << std::endl;

	//Let's reflect it
	astra::TypeInfo info = astra::reflect(&car);

	//Let's see if the car is insured
	auto insuredVar = info.get<astra::Object>().getField("insured").unwrap().var();
	bool insured = *(insuredVar.rtCast<bool>().unwrap());
	std::cout << "According to reflection, car is " << (insured ? "" : "NOT ") << "insured" << std::endl;

	//Give the car insurance now
	astra::reflect(insuredVar).get<astra::Bool>().set(true);
	std::cout << "THE INSURANCE WIZARD HATH SPOKEN! ABRA-CADRABA-INSURANCE!" << std::endl;
	insured = *(insuredVar.rtCast<bool>().unwrap());
	std::cout << "According to reflection, car is " << (insured ? "" : "NOT ") << "insured" << std::endl;

	//What color is the car?
	Color carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).get<astra::Enum>().toString() << std::endl;

	//Let's recolor the car
	car.refinish(Color::Black);
	std::cout << "Imma recolor the car." << std::endl;
	carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).get<astra::Enum>().toString() << std::endl;

	//By the way, sat radio?
	std::cout << "By the way, the car does " << (car.hasSatRadio ? "" : "NOT ") << "have satellite radio. Reflection just can't see it because we marked it as ignored." << std::endl;

	//Recolor the car via reflection
	std::cout << "Anyways, let's recolor the car with reflection" << std::endl;
	int cost = info.get<astra::Object>().getMethod("refinish").unwrap().invoke<int>(Color::Yellow).unwrap();
	carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).get<astra::Enum>().toString() << std::endl;
	std::cout << "Refinishing it cost $" << cost << " though :(" << std::endl;

	//Direct private field access
	std::cout << "Why not do it for free? Unleash the direct private field access!" << std::endl;
	astra::reflect(info.get<astra::Object>().getField("color").unwrap().var()).get<astra::Enum>().fromString("White").unwrap();
	carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).get<astra::Enum>().toString() << std::endl;

	//SUV for base class testing
	ExampleNamespace::SUV suv;
	suv.brand = "Subaru";
	suv.year = 2016;
	suv.hasInsurance = true;
	suv.hasSatRadio = false;
	suv.nickname = "Macho Truck";
	suv.refinish(Color::Red);
	suv.owner = "Big Mack";
	suv.trunkVolume = 4.5f * 3.1f * 2.0f;
	std::cout << "I have an SUV now:\n"
			  << astra::json::toString(&suv).unwrap() << std::endl;
	auto suvInfo = astra::reflect(&suv);
	std::cout << "Because an SUV is also a Car... I can do car stuff with it." << std::endl;
	std::cout << "Let's refinish it via reflection!" << std::endl;
	int suvCost = suvInfo.get<astra::Object>().getMethod("refinish").unwrap().invoke<int>(Color::Blue).unwrap();
	Color suvColor = suv.whatColorAmI();
	std::cout << "The SUV is " << astra::reflect(&suvColor).get<astra::Enum>().toString() << std::endl;
	std::cout << "Refinishing it cost more: $" << suvCost << std::endl;
	std::cout << "I also can do SUV-specific stuff, like checking the trunk volume" << std::endl;
	std::cout << "(it's " << astra::reflect(suvInfo.get<astra::Object>().getField("trunkVolume").unwrap().var()).get<astra::Floating>().get() << " cubic feet btw)" << std::endl;

	//Casting to base class
	std::cout << "Let's cast the SUV back to a car." << std::endl;
	Car* suvAsCar = static_cast<Car*>(&suv);
	std::cout << "Serializing the reflected Car pointer gives us:\n"
			  << astra::json::toString(suvAsCar).unwrap() << std::endl;
	std::cout << "As you can see, it still has the SUV information." << std::endl;

	//Done
	std::cout << "Thanks for checking out the Astra demo!" << std::endl;

	return 0;
}