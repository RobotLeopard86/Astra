#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <utility>

#include "astra/expected.hpp"
#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "astra/variable/var.hpp"
#include "fields.hpp"
#include "methods.hpp"

namespace astra {

	///container of all field names matched to Var structs
	///just a registry of types and value pointers
	struct Object {
		Object(Var var,											//
			const std::map<std::string_view, FieldDesc>* fields,//
			const std::map<std::string_view, MethodDesc>* methods)
		  : _var(var),		//
			_fields(fields),//
			_methods(methods) {
		}

		Expected<None> assign(Var var) {
			if(var.type() != _var.type()) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::typeName(var.type()),					  //
					reflection::typeName(_var.type())));
			}
			_var = var;

			return None();
		}

		void unsafeAssign(void* ptr) {
			_var.unsafeAssign(ptr);
		}

		Expected<FieldInfo> getField(std::string_view name) {
			auto it = _fields->find(name);

			if(it != _fields->end()) {
				return FieldInfo(_var.raw(), &it->second);
			}
			return Error(astra::format("There is no field with name: '{}'", name));
		}

		Fields getFields(Access access = Access::kPublic, bool includeReadonly = false) const {
			return Fields(_var.raw(), _fields, access, includeReadonly);
		}

		Expected<MethodInfo> getMethod(std::string_view name) {
			auto it = _methods->find(name);

			if(it != _methods->end()) {

				if(_var.isConst()) {
					if(it->second.isConst()) {
						return MethodInfo(_var.raw(), &it->second);
					}
					return Error(astra::format("Cannot call non const method '{}' on const object", name));
				}

				return MethodInfo(_var.rawMut(), &it->second);
			}
			return Error(astra::format("There is no method with name: '{}'", name));
		}

		Var var() {
			return _var;
		}

	  private:
		Var _var;
		const std::map<std::string_view, FieldDesc>* _fields;
		const std::map<std::string_view, MethodDesc>* _methods;
	};

}
