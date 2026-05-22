#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <utility>


#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "astra/var.hpp"
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

		void assign(Var var) {
			if(var.type() != _var.type()) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(_var.type())));
			}
			_var = var;

			return;
		}

		void unsafeAssign(void* ptr) {
			_var.unsafeAssign(ptr);
		}

		FieldInfo getField(std::string_view name) {
			auto it = _fields->find(name);

			if(it != _fields->end()) {
				return FieldInfo(_var.raw(), &it->second);
			}
			throw std::runtime_error(format("There is no field with name: '{}'", name));
		}

		Fields getFields(Access access = Access::kPublic, bool includeReadonly = false) const {
			return Fields(_var.raw(), _fields, access, includeReadonly);
		}

		MethodInfo getMethod(std::string_view name) {
			auto it = _methods->find(name);

			if(it != _methods->end()) {

				if(_var.isConst()) {
					if(it->second.isConst()) {
						return MethodInfo(_var.raw(), &it->second);
					}
					throw std::runtime_error(format("Cannot call non const method '{}' on const object", name));
				}

				return MethodInfo(_var.rawMut(), &it->second);
			}
			throw std::runtime_error(format("There is no method with name: '{}'", name));
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
