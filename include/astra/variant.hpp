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
		  : _content(value) {
		}

		template<typename U, typename = std::enable_if_t<!std::is_same_v<U, Variant>, void>>
		Variant(U&& value)
		  : _content(std::move(value)) {
		}

		template<typename... FuncT>
		decltype(auto) match(FuncT... functions) {
			return std::visit(Overloaded {functions...}, _content);
		}

		template<typename... FuncT>
		decltype(auto) matchMove(FuncT... functions) {
			return std::visit(Overloaded {functions...}, std::move(_content));
		}

		template<typename... FuncT>
		decltype(auto) match(FuncT... functions) const {
			return std::visit(Overloaded {functions...}, _content);
		}

		template<typename U>
		[[nodiscard]] inline bool is() const {
			return std::holds_alternative<U>(_content);
		}

		[[nodiscard]] inline std::size_t index() const {
			return _content.index();
		}

		template<typename U>
		U& as() {
			auto pointer = std::get_if<U>(&_content);

			if(pointer == nullptr) {
				throw std::runtime_error("The instance of requested type doesn't exist");
			}
			return *pointer;
		}

		template<typename U>
		inline const U& asUnsafe() const {
			return std::get<U>(_content);
		}

		template<typename U>
		inline U& asUnsafe() {
			return std::get<U>(_content);
		}

	  protected:
		std::variant<T...> _content;
	};

}