#pragma once

#include <cstddef>

#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/variant.hpp"
#include "istring.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API CString : IString {

		CString(const T** str)
		  : inner(str, true) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) override {
			if(var.typeId() != inner.typeId()) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(inner.typeId())));
			}
			inner = var;
			cachedVal = static_cast<const char*>(inner.raw());
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) override {
			inner.unsafeAssign(ptr);
			cachedVal = *static_cast<const char*>(inner.raw());
		}

		const std::string& get() const override {
			return cachedVal;
		}

		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(const std::string&) override {
			throw std::runtime_error("Trying to set read-only value");
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() const override {
			return inner;
		}

	  private:
		Var inner;
		std::string cachedVal;
	};

}