#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <utility>

#include "astra/dll.hpp"
#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "astra/var.hpp"
#include "fields.hpp"
#include "methods.hpp"

namespace astra {

	///container of all field names matched to Var structs
	///just a registry of types and value pointers
	struct ASTRA_API Object {
		Object(Var var,											//
			const std::map<std::string_view, FieldDesc>* fields,//
			const std::map<std::string_view, MethodDesc>* methods)
		  : inner(var),	   //
			fields(fields),//
			methods(methods) {
		}

		void assign(Var var) {
			if(var.typeId() != inner.typeId()) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.typeId()),												//
					typeName(inner.typeId())));
			}
			inner = var;

			return;
		}

		void unsafeAssign(void* ptr) {
			inner.unsafeAssign(ptr);
		}

		FieldInfo getField(const std::string& name) {
			auto it = fields->find(name);

			if(it != fields->end()) {
				return FieldInfo(inner.raw(), &it->second);
			}
			throw std::runtime_error(::astra::format("There is no field with name: '{}'", name));
		}

		Fields getFields(Access access = Access::Public, bool includeReadonly = false) const {
			return Fields(inner.raw(), fields, access, includeReadonly);
		}

		MethodInfo getMethod(const std::string& name) {
			auto it = methods->find(name);

			if(it != methods->end()) {

				if(inner.isConst()) {
					if(it->second.isConst()) {
						return MethodInfo(inner.raw(), &it->second);
					}
					throw std::runtime_error(::astra::format("Cannot call non const method '{}' on const object", name));
				}

				return MethodInfo(inner.rawMut(), &it->second);
			}
			throw std::runtime_error(::astra::format("There is no method with name: '{}'", name));
		}

		Var var() {
			return inner;
		}

	  private:
		Var inner;
		const std::map<std::string_view, FieldDesc>* fields;
		const std::map<std::string_view, MethodDesc>* methods;
	};
}
