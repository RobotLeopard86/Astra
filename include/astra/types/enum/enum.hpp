#pragma once

#include <memory>

#include "enum_impl.hpp"
#include "ienum.hpp"
#include "astra/dll.hpp"

namespace astra {
	/**
	 * @brief Reflection interface for an enumeration
	 */
	class ASTRA_API Enum final {
	  public:
		/**
		 * @brief Create a new enum from data and a values list
		 *
		 * @tparam T The enum type
		 * @tparam size The size of the intializer array
		 *
		 * @param value The enum object to operate on
		 * @param isConst If write operations should be disabled
		 * @param allConstants A map of enum values to their string names
		 */
		template<typename T, std::size_t size_v>
		Enum(T* value, bool isConst, const ConstexprMap<T, size_v>& allConstants) {
			new(mem) EnumImpl<T, size_v>(value, isConst, allConstants);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain an enum of matching type
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