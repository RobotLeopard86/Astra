#pragma once

#include "astra/reflectable.hpp"
#include "astra/type_id.hpp"
#include "astra/type_info/type_info.hpp"

namespace astra {

	TypeInfo reflect(Var variable);

	template<Reflectable T>
	TypeInfo reflect(T* pointer) {
		return reflect(Var(pointer));
	}

	template<Reflectable T>
	TypeInfo reflect(const T* pointer) {
		return reflect(Var(pointer));
	}

	std::string sprint(const TypeInfo& info);
	std::string sprint(Var var);

	template<Reflectable T>
	std::string sprint(const T* pointer) {
		return sprint(reflect(pointer));
	}

	void print(const TypeInfo& info);
	void print(Var var);

	template<Reflectable T>
	void print(const T* pointer) {
		print(reflect(pointer));
	}

	std::size_t typeSize(TypeId id);

	void construct(Var variable);
	void destroy(Var variable);

	Expected<None> copy(Var to, Var from);

}
