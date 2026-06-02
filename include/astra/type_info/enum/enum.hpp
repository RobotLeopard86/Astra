#pragma once

#include <memory>

#include "enum_impl.hpp"
#include "ienum.hpp"
#include "astra/dll.hpp"

namespace astra {

	///container of all field names matched to Var structs
	///just a registry of types and value pointers
	class ASTRA_API Enum final {
	  public:
		template<typename T, std::size_t size>
		Enum(T* value, bool isConst, const ConstexprMap<T, size>& allConstants) {
			new(mem) EnumImpl<T, size>(value, isConst, allConstants);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) {
			return impl()->assign(var);
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() const {
			return impl()->var();
		}

		/**
		 * @brief Get the name of the current enumeration value
		 *
		 * @return The name of the current value
		 */
		const std::string& toString() const {
			return impl()->toString();
		}

		/**
		 * @brief Set the current enumeration value by name
		 *
		 * @param name The name of the value to store
		 */
		void fromString(const std::string& name) {
			return impl()->fromString(name);
		}

	  private:
		char mem[sizeof(EnumImpl<int, 0>)];

		inline const IEnum* impl() const {
			return reinterpret_cast<const IEnum*>(&mem[0]);
		}

		inline IEnum* impl() {
			return reinterpret_cast<IEnum*>(&mem[0]);
		}
	};

}