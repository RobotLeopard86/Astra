#pragma once

#include <stdexcept>
#include <type_traits>
#include <variant>

#include "overloaded.hpp"
#include "dll.hpp"

namespace astra {
	/**
	 * @brief Extended variant object wrapping @c std::variant for extra functionality
	 *
	 * @tparam Ts The possible stored types
	 */
	template<typename... T>
	struct ASTRA_API Variant {
		/**
		 * @brief Create a new variant by copying in a value
		 *
		 * @tparam U The type of the value being passed in
		 *
		 * @param value The value to store in the variant
		 */
		template<typename U, typename = std::enable_if_t<!std::is_same_v<U, Variant>, void>>
		Variant(const U& value)
		  : content(value) {
		}

		/**
		 * @brief Create a new variant by moving in a value
		 *
		 * @tparam U The type of the value being passed in
		 *
		 * @param value The value to store in the variant
		 */
		template<typename U, typename = std::enable_if_t<!std::is_same_v<U, Variant>, void>>
		Variant(U&& value)
		  : content(std::move(value)) {
		}

		template<typename... F>
		decltype(auto) match(F... functions) {
			return std::visit(Overloaded {functions...}, content);
		}

		template<typename... F>
		decltype(auto) matchMove(F... functions) {
			return std::visit(Overloaded {functions...}, std::move(content));
		}

		template<typename... F>
		decltype(auto) match(F... functions) const {
			return std::visit(Overloaded {functions...}, content);
		}

		/**
		 * @brief Check if the variant holds a given type
		 *
		 * @tparam U The type to check for
		 *
		 * @return If an object of type U is contained
		 */
		template<typename U>
		[[nodiscard]] inline bool is() const {
			return std::holds_alternative<U>(content);
		}

		/**
		 * @brief Get the index of the type stored in the variant (0-based in order of template arguments)
		 *
		 * @return The index of the currently stored type
		 */
		[[nodiscard]] inline std::size_t index() const {
			return content.index();
		}

		/**
		 * @brief Access the contents of the variant as a specific type
		 *
		 * @tparam U The type to output as
		 *
		 * @return A reference to the contents
		 */
		template<typename U>
		U& as() {
			auto pointer = std::get_if<U>(&content);
			if(pointer == nullptr) {
				throw std::runtime_error("The instance of requested type doesn't exist");
			}
			return *pointer;
		}

		/**
		 * @brief Access the contents of the variant constly without type-checking
		 *
		 * @warning This method is for internal use only!
		 *
		 * @tparam U The type to output as
		 *
		 * @return A reference to the contents
		 */
		template<typename U>
		inline const U& asUnsafe() const {
			return std::get<U>(content);
		}

		/**
		 * @brief Access the contents of the variant without type-checking
		 *
		 * @warning This method is for internal use only!
		 *
		 * @tparam U The type to output as
		 *
		 * @return A reference to the contents
		 */
		template<typename U>
		inline U& asUnsafe() {
			return std::get<U>(content);
		}

	  protected:
		std::variant<T...> content;
	};

}