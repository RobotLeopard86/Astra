#pragma once

#include <cctype>
#include <string_view>

#include "astra/dll.hpp"
#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/type_id.hpp"
#include "astra/var.hpp"

namespace astra {
	/**
	 * @brief Reflected boolean interface
	 */
	struct ASTRA_API Bool {
		/**
		 * @brief Create a new Bool
		 *
		 * @param value A pointer to the bool to hold (does not take ownership)
		 * @param isConst If write operations should be disabled
		 */
		Bool(bool* value, bool isConst)
		  : value(value), isConst(isConst) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) {
			auto t = TypeId::get<bool>();
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			value = const_cast<bool*>(static_cast<const bool*>(var.raw()));
			isConst = var.isConst();
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) {
			value = static_cast<bool*>(ptr);
			isConst = false;
		}

		/**
		 * @brief Get the stored value
		 *
		 * @return The stored value
		 */
		bool get() const {
			return *value;
		}

		/**
		 * @brief Set the stored value
		 *
		 * @param val The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(bool val) {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}
			*value = val;
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() {
			return Var(value, isConst);
		}

	  private:
		bool* value;
		bool isConst;
	};

}