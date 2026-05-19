#include "astra/reflection/reflection.hpp"

#include <cstddef>
#include <iostream>
#include <memory_resource>

#include "astra/reflection/the_great_table.hpp"
#include "astra/types/all_types.hpp"
#include "sprint.hpp"

using namespace astra;

TypeInfo reflection::reflect(Var variable) {
	return TheGreatTable::data()[variable.type().number()].reflect(const_cast<void*>(variable.raw()),
		variable.isConst());
}

std::string reflection::sprint(const TypeInfo& info) {
	std::string result;
	sprint(info, &result, 0);
	return result;
}

std::string reflection::sprint(Var var) {
	return sprint(reflect(var));
}

void reflection::print(const TypeInfo& info) {
	std::cout << sprint(info) << std::flush;
}

void reflection::print(Var var) {
	print(reflect(var));
}

std::string_view reflection::typeName(TypeId id) {
	return TheGreatTable::data()[id.number()].typeName();
}

#ifndef NDEBUG
std::string_view reflection::typeName(uint32_t id) {
	return TheGreatTable::data()[id].typeName();
}
#endif

std::size_t reflection::typeSize(TypeId id) {
	return TheGreatTable::data()[id.number()].typeSize();
}

void reflection::construct(Var variable) {
	return TheGreatTable::data()[variable.type().number()].construct(variable.rawMut());
}

void reflection::destroy(Var variable) {
	if(variable.raw() == nullptr) {
		return;
	}
	TheGreatTable::data()[variable.type().number()].destroy(variable.rawMut());
}

Expected<None> reflection::copy(Var to, Var from) {
	if(to.isConst()) {
		return Error("Cannot assign to const value");
	}
	if(to.type() != from.type()) {
		return Error(astra::format("Cannot copy {} to {}", typeName(from.type()), typeName(to.type())));
	}
	TheGreatTable::data()[to.type().number()].copy(to.rawMut(), from.raw());
	return None();
}
