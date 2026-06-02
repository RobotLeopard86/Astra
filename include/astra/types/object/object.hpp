#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <utility>

#include "astra/dll.hpp"
#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/var.hpp"
#include "field_group.hpp"
#include "method_group.hpp"

namespace astra {

	///container of all field names matched to Var structs
	///just a registry of types and value pointers
	class ASTRA_API Object {
	  public:
		Object(Var var,
			const std::map<std::string_view, FieldDesc>* fields,
			const std::map<std::string_view, MethodDesc>* methods)
		  : inner(var),
			fields(fields),
			methods(methods) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) {
			if(var.typeId() != inner.typeId()) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(inner.typeId())));
			}
			inner = var;
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) {
			inner.unsafeAssign(ptr);
		}

		/**
		 * @brief Access a field by name
		 *
		 * @param name The name of the field to access
		 *
		 * @return The FieldInfo for the requested field
		 *
		 * @throws std::runtime_error If no field with the provided name exists in the object
		 */
		FieldInfo getField(const std::string& name) {
			auto it = fields->find(name);

			if(it != fields->end()) {
				return FieldInfo(inner.raw(), &it->second);
			}
			throw std::runtime_error(::astra::format("There is no field with name: '{}'", name));
		}

		/**
		 * @brief Get all fields in the object with the given access and read-only state that may be accessed on a const object
		 *
		 * @param access The access specifier for fields to query
		 * @param includeReadonly Whether or not to include read-only fields in the group
		 *
		 * @return The matching group of fields
		 */
		FieldGroup getFields(Access access = Access::Public, bool includeReadonly = true) const {
			return FieldGroup(inner.raw(), fields, access, includeReadonly);
		}

		/**
		 * @brief Get all fields in the object with the given access and read-only state
		 *
		 * @param access The access specifier for fields to query
		 * @param includeReadonly Whether or not to include read-only fields in the group
		 *
		 * @return The matching group of fields
		 */
		FieldGroup getFields(Access access = Access::Public, bool includeReadonly = false) {
			return FieldGroup(inner.rawMut(), fields, access, includeReadonly);
		}

		/**
		 * @brief Access a method by name
		 *
		 * @param name The name of the method to access
		 *
		 * @return The MethodInfo for the requested method
		 *
		 * @throws std::runtime_error If no method with the provided name exists in the object
		 */
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

		/**
		 * @brief Get all methods in the object with the given access flags that may be accessed on a const object
		 *
		 * @param access The access specifier for methods to query
		 *
		 * @return The matching group of methods
		 */
		MethodGroup getMethods(Access access = Access::Public) const {
			return MethodGroup(inner.raw(), methods, access);
		}

		/**
		 * @brief Get all methods in the object with the given access flags
		 *
		 * @param access The access specifier for methods to query
		 *
		 * @return The matching group of methods
		 */
		MethodGroup getMethods(Access access = Access::Public) {
			return MethodGroup(inner.rawMut(), methods, access);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() {
			return inner;
		}

	  private:
		Var inner;
		const std::map<std::string_view, FieldDesc>* fields;
		const std::map<std::string_view, MethodDesc>* methods;
	};
}
