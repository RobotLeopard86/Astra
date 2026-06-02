#pragma once

#include "dll.hpp"
#include "reflectable.hpp"
#include "type_id.hpp"
#include <stdexcept>

namespace astra {
	/**
	 * @brief Type-agnostic storage mechanism wrapping value, type ID, and const-ness
	 */
	struct ASTRA_API Var {
		/**
		 * @brief Create an empty Var
		 */
		Var();

		/**
		 * @brief Create a Var holding some typed data
		 *
		 * @note The Var does not maintain ownership over the passed pointer; it is the caller's responsibility to manage the pointer's lifetime externally
		 *
		 * @param value The value to store (may be @c nullptr)
		 * @param type The type to associate with the Var
		 * @param isConst Whether or not the Var should be const and disallow write operations
		 */
		Var(void* value, TypeId type, bool isConst);

		/**
		 * @brief Create a Var holding an existing const object
		 *
		 * @note The Var does not maintain ownership over the passed pointer; it is the caller's responsibility to manage the pointer's lifetime externally
		 *
		 * @param value The value to store (may not be @c nullptr)
		 */
		template<typename T>
		explicit Var(const T* value)
		  : value(value == nullptr ? throw std::runtime_error("Cannot create Var with type deduction using a nullptr!") : const_cast<T*>(value)), type(TypeId::get<T>()), areWeConst(true) {
		}

		/**
		 * @brief Create a Var holding an existing object
		 *
		 * @note The Var does not maintain ownership over the passed pointer; it is the caller's responsibility to manage the pointer's lifetime externally
		 *
		 * @param value The value to store (may not be @c nullptr)
		 * @param isConst Whether or not the Var should be const and disallow write operations
		 */
		template<typename T>
		explicit Var(T* value, bool isConst = false)
		  : value(value == nullptr ? throw std::runtime_error("Cannot create Var with type deduction using a nullptr!") : value), type(TypeId::get(value)), areWeConst(isConst) {
		}

		///@cond
		template<Reflectable T>
			requires(!std::is_enum_v<T> && std::is_class_v<T>)
		explicit Var(const T* value)
		  : value(const_cast<T*>(value)), type(value != nullptr ? value->ASTRA__gettypeid() : TypeId::get<T>()), areWeConst(true) {}

		template<Reflectable T>
			requires(!std::is_enum_v<T> && std::is_class_v<T>)
		explicit Var(T* value, bool isConst = false)
		  : value(value), type(value != nullptr ? value->ASTRA__gettypeid() : TypeId::get<T>()), areWeConst(isConst) {}
		///@endcond

		/**
		 * @brief Overwrite the Var's internal storage with the provided pointer
		 *
		 * @warning <b>This function is designed for internal use only and can cause serious memory management issues if used incorrectly! Use with caution!</b>
		 *
		 * @param ptr The pointer to store
		 */
		void unsafeAssign(void* ptr);

		/**
		 * @brief Get a mutable raw pointer to the contained object
		 *
		 * @warning <b>This function is designed for internal use only and can cause serious memory management issues if used incorrectly! Use with caution!</b>
		 *
		 * @return The contained object, or @c nullptr if the Var is const
		 */
		void* rawMut();

		/**
		 * @brief Get a const raw pointer to the contained object
		 *
		 * @warning <b>This function is designed for internal use only and can cause serious memory management issues if used incorrectly! Use with caution!</b>
		 *
		 * @return The contained object
		 */
		const void* raw() const;

		/**
		 * @brief Get the TypeId associated with the object in the Var
		 *
		 * @return The associated type ID
		 */
		TypeId typeId() const;

		/**
		 * @brief Check whether or not the Var is const
		 *
		 * @return If the var is const or not
		 */
		bool isConst() const;

		/**
		 * @brief Release the pointer from the Var and clear state; resets to an empty var
		 *
		 * @return The previously-held pointer
		 */
		void* release();

		/**
		 * @brief Safely access the contents of the Var through a real type
		 *
		 * @tparam T The type to return the contents as
		 *
		 * @return The stored contents; will be a const pointer if T is const and a mutable pointer if T is mutable
		 *
		 * @throws std::runtime_error If this method is called on a const Var with a non-const pointer return type
		 */
		template<typename T>
		T* rtCast() const {
			if(!std::is_const_v<T> && areWeConst) {
				throw std::runtime_error("Cannot return mutable reference to const Var!");
			}

			auto desiredType = TypeId::get<std::remove_cvref_t<T>>();
			if(desiredType != type) {
				error(type, desiredType);
			}

			return static_cast<T*>(value);
		}

		///@cond
		bool operator==(const Var& other) const;
		bool operator!=(const Var& other) const;
		///@endcond

	  private:
		void* value;
		TypeId type;
		bool areWeConst;

		//include reflection header into .cpp file to avoid cyclic dependencies
		static void error(TypeId type, TypeId desiredType);
	};

}
