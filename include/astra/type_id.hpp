#pragma once

#include "traits.hpp"
#include "dll.hpp"

#include <cstdint>

namespace astra {
	/**
	 * @brief Sequential type ID wrapper and lookup system
	 */
	struct ASTRA_API TypeId {
		/**
		 * @brief Create a default type ID (represents unknown type)
		 */
		TypeId()
		  : id(0) {
		}

		/**
		 * @brief Get the type ID for a given type
		 *
		 * @tparam T The type to lookup
		 *
		 * @return The type ID
		 */
		template<typename T>
		static TypeId get() {
			return TypeId(0);
		}

		/**
		 * @brief Get the type ID for a given object
		 *
		 * @tparam T The type of the object (implicit)
		 *
		 * @return The type ID
		 */
		template<typename T>
		static TypeId get(T*) {
			return TypeId::get<T>();
		}

		/**
		 * @brief Get the stored type ID number
		 *
		 * @return The type ID number
		 */
		uint32_t number() const {
			return id;
		}

		///@cond
		template<typename T>
			requires(requires(T t) {
				{ t.ASTRA__gettypeid() } -> std::same_as<TypeId>;
			})
		static TypeId get(T* ptr) {
			return ptr->ASTRA__gettypeid();
		}

		template<>
		TypeId get<bool>() {
			return getBoolId();
		}

		template<typename T>
			requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
		static TypeId get();

		template<typename T>
			requires std::is_floating_point_v<T>
		static TypeId get();

		template<typename T>
			requires is_string_v<T>
		static TypeId get();

		template<typename T>
			requires is_array_v<T>
		static TypeId get();

		template<typename T>
			requires is_std_array_v<T>
		static TypeId get();

		template<typename T>
			requires is_list_v<T>
		static TypeId get();

		template<typename T>
			requires is_map_v<T>
		static TypeId get();

		template<typename T>
			requires is_shared_ptr_v<T>
		static TypeId get();

		template<typename T>
			requires is_unique_ptr_v<T>
		static TypeId get();

		template<typename T>
			requires std::is_enum_v<T>
		static TypeId get() {
			return TypeId(0);
		}

		template<typename T>
			requires std::is_void_v<T>
		static TypeId get() {
			return TypeId(0);
		}

		bool operator==(const TypeId& other) const {
			return id == other.id;
		}

		bool operator!=(const TypeId& other) const {
			return id != other.id;
		}
		///@endcond
	  private:
		uint32_t id;

		static TypeId getBoolId();

		explicit TypeId(uint32_t id)
		  : id(id) {}
	};
}
