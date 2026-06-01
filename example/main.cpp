#include "data/complicatedtype.hpp"
#include "data/car.hpp"
#include "data/colors.hpp"
#include "data/suv.hpp"

#include "example.astra.hpp"

#include "astra/reflection.hpp"
#include "astra/json.hpp"
#include "astra/yaml.hpp"
#include "astra/binary.hpp"
#include "astra/serialized_convert.hpp"

#include <fstream>
#include <iostream>
#include <iterator>

int main() {
	//We have a car
	Car car = {};
	car.brand = "Toyota";
	car.year = 2023;
	car.owner = "John Smith";

	//Let's dump it to the console as JSON
	std::string asYaml = astra::yaml::toString(&car);
	std::cout << "I have a car:\n---\n"
			  << asYaml << "\n---" << std::endl;

	//Let's turn the JSON into YAML
	std::cout << "This can also be in JSON: "
			  << astra::convert::yamlStringToJsonString(asYaml) << std::endl;

	//Let's reflect it
	astra::TypeInfo info = astra::reflect(&car);

	//Let's see if the car is insured
	auto insuredVar = info.as<astra::Object>().getField("insured").var();
	bool insured = *(insuredVar.rtCast<bool>());
	std::cout << "According to reflection, car is " << (insured ? "" : "NOT ") << "insured" << std::endl;

	//Give the car insurance now
	astra::reflect(insuredVar).as<astra::Bool>().set(true);
	std::cout << "THE INSURANCE WIZARD HATH SPOKEN! ABRA-CADRABA-INSURANCE!" << std::endl;
	insured = *(insuredVar.rtCast<bool>());
	std::cout << "According to reflection, car is " << (insured ? "" : "NOT ") << "insured" << std::endl;

	//What color is the car?
	Color carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).as<astra::Enum>().toString() << std::endl;

	//Let's recolor the car
	car.refinish(Color::Black);
	std::cout << "Imma recolor the car." << std::endl;
	carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).as<astra::Enum>().toString() << std::endl;

	//By the way, sat radio?
	std::cout << "By the way, the car does " << (car.hasSatRadio ? "" : "NOT ") << "have satellite radio. Reflection just can't see it because we marked it as ignored." << std::endl;

	//Recolor the car via reflection
	std::cout << "Anyways, let's recolor the car with reflection" << std::endl;
	int cost = info.as<astra::Object>().getMethod("refinish").invoke<int>(Color::Yellow);
	carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).as<astra::Enum>().toString() << std::endl;
	std::cout << "Refinishing it cost $" << cost << " though :(" << std::endl;

	//Direct private field access
	std::cout << "Why not do it for free? Unleash the direct private field access!" << std::endl;
	astra::reflect(info.as<astra::Object>().getField("color").var()).as<astra::Enum>().fromString("White");
	carColor = car.whatColorAmI();
	std::cout << "The car is " << astra::reflect(&carColor).as<astra::Enum>().toString() << std::endl;

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
			  << astra::json::toString(&suv) << std::endl;
	auto suvInfo = astra::reflect(&suv);
	std::cout << "Because an SUV is also a Car... I can do car stuff with it." << std::endl;
	std::cout << "Let's refinish it via reflection!" << std::endl;
	int suvCost = suvInfo.as<astra::Object>().getMethod("refinish").invoke<int>(Color::Blue);
	Color suvColor = suv.whatColorAmI();
	std::cout << "The SUV is " << astra::reflect(&suvColor).as<astra::Enum>().toString() << std::endl;
	std::cout << "Refinishing it cost more: $" << suvCost << std::endl;
	std::cout << "I also can do SUV-specific stuff, like checking the trunk volume" << std::endl;
	std::cout << "(it's " << astra::reflect(suvInfo.as<astra::Object>().getField("trunkVolume").var()).as<astra::Float>().get() << " cubic feet btw)" << std::endl;

	//Casting to base class
	std::cout << "Let's cast the SUV back to a car." << std::endl;
	Car* suvAsCar = static_cast<Car*>(&suv);
	std::cout << "Serializing the reflected Car pointer gives us:\n"
			  << astra::json::toString(suvAsCar) << std::endl;
	std::cout << "As you can see, it still has the SUV information." << std::endl;

	//Write out the complicated type to binary
	std::cout << "For my last magic trick, I will write a very complicated type to a binary file!" << std::endl;
	ExampleNamespace::ComplicatedType complicated;
	complicated.myString = "Lorem impsum sit dolor amet i do not know how to write lorem ipsum ahhhhhhh";
	complicated.ptrToAnotherEnum = std::make_unique<ExampleNamespace::ComplicatedType::AnotherEnum>(ExampleNamespace::ComplicatedType::AnotherEnum::ArrozConPollo);
	complicated.carrrrr = std::make_unique<std::shared_ptr<Car>>(std::make_shared<Car>());
	(*complicated.carrrrr)->brand = "Toronto Motors";
	(*complicated.carrrrr)->owner = "Lord Moldevort";
	(*complicated.carrrrr)->hasInsurance = false;
	(*complicated.carrrrr)->hasSatRadio = false;
	(*complicated.carrrrr)->year = 1995;
	(*complicated.carrrrr)->refinish(Color::Purple);
	complicated.someMap[Color::Red] = {{1, 2, 3, 4, 5}};
	complicated.someMap[Color::Orange] = {{10, 9, 8, 7, 6}};
	complicated.someMap[Color::Green] = {{31, 41, 59, 26, 53}};
	complicated.someMap[Color::Black] = {{1, 7, 7, 6, 0}};
	complicated.listOfLists.resize(2);
	complicated.listOfLists[0] = std::array<bool, 3> {{true, false, true}};
	complicated.listOfLists[0] = std::array<bool, 3> {{false, false, true}};
	std::ofstream ofs("./complicated.xj", std::ios::binary);
	astra::binary::toStream(ofs, &complicated);
	ofs.close();

	//Get it back
	std::cout << "And finally, let's get the complicated type back!" << std::endl;
	std::ifstream ifs("./complicated.xj", std::ios::binary);
	std::string complicatedJson = astra::convert::binaryStreamToJsonString(ifs);
	std::cout << "In JSON, we can write it out as: " << complicatedJson << std::endl;
	std::ofstream of2("./complicated2.xj", std::ios::binary);
	std::vector<unsigned char> c = astra::convert::jsonStringToBinaryVec(complicatedJson);
	std::ranges::copy(c, std::ostreambuf_iterator<char>(of2));
	std::cout << "I turned it back to binary and put it in another file. Tee-hee!" << std::endl;

	//Done
	std::cout << "Thanks for checking out the Astra demo!" << std::endl;

	return 0;
}