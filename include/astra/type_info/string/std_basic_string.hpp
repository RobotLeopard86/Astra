#pragma once

#include <cstddef>

#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/variant.hpp"
#include "istring.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T>
	class ASTRA_API StdBasicString : IString {
	  public:
		StdBasicString() = delete;

		StdBasicString(std::basic_string<T>* str, bool isConst)
		  : inner(str, isConst) {
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
		}

		const std::string& get() const override {
			return *static_cast<const std::string*>(inner.raw());
		}

		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(const std::string& value) override {
			if(inner.isConst()) {
				throw std::runtime_error("Trying to set const value");
			}
			*static_cast<std::string*>(inner.rawMut()) = value;
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
	};

}
///@endcond
