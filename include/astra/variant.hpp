#pragma once

#include <stdexcept>
#include <type_traits>
#include <variant>

#include "overloaded.hpp"
#include "dll.hpp"

namespace astra {

	template<typename... T>
	struct ASTRA_API Variant {

		template<typename U, typename = std::enable_if_t<!std::is_same_v<U, Variant>, void>>
		Variant(const U& value)
		  : content(value) {
		}

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

		template<typename U>
		[[nodiscard]] inline bool is() const {
			return std::holds_alternative<U>(content);
		}

		[[nodiscard]] inline std::size_t index() const {
			return content.index();
		}

		template<typename U>
		U& as() {
			auto pointer = std::get_if<U>(&content);

			if(pointer == nullptr) {
				throw std::runtime_error("The instance of requested type doesn't exist");
			}
			return *pointer;
		}

		template<typename U>
		inline const U& asUnsafe() const {
			return std::get<U>(content);
		}

		template<typename U>
		inline U& asUnsafe() {
			return std::get<U>(content);
		}

	  protected:
		std::variant<T...> content;
	};

}