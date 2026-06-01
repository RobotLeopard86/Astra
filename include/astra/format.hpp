#pragma once

#include <cstddef>
#include <filesystem>
#include <iomanip>
#include <string>
#include <type_traits>

#include "traits.hpp"
#include "dll.hpp"

namespace astra {
	///@cond
	template<typename T>
		requires is_string_v<std::remove_cvref_t<T>>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg;
	}

	template<typename T>
		requires std::is_array_v<std::remove_cvref_t<T>> && std::is_same_v<array_value_t<T>, const char>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg;
	}

	template<typename T>
		requires std::is_integral_v<std::remove_cvref_t<T>> && (!std::is_same_v<std::remove_cvref_t<T>, bool>) && (!std::is_same_v<std::remove_cvref_t<T>, char>)
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += std::to_string(arg);
	}

	template<typename T>
		requires std::is_floating_point_v<std::remove_cvref_t<T>>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		std::ostringstream stream(*str);
		stream << std::setiosflags(std::ios::fixed) << std::setprecision(2);
		stream << arg;
	}

	template<typename T>
		requires std::is_same_v<std::remove_cvref_t<T>, bool>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg ? "true" : "false";
	}

	template<typename T>
		requires std::is_same_v<std::remove_cvref_t<T>, char>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg;
	}

	template<typename T>
		requires std::is_same_v<std::remove_cvref_t<T>, std::filesystem::path>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg.string();
	}

	[[maybe_unused]] inline void format(std::string* result, std::string_view fmt, std::size_t i) {
		*result += fmt.substr(i, fmt.size() - i);
	}

	template<typename T, typename... Ts>
	ASTRA_API inline void format(std::string* result, std::string_view fmt, std::size_t i, T&& arg, Ts&&... args) {
		auto pos = fmt.find('{', i);

		if(pos == std::string::npos) {
			*result += fmt.substr(i, fmt.size() - i);
			return;
		}
		*result += fmt.substr(i, pos - i);
		append(result, std::forward<T>(arg));
		format(result, fmt, pos + 2, std::forward<Ts>(args)...);
	}
	///@endcond

	/**
	 * @brief Formats a string using the provided format string and arguments.
	 * @details This is the public-facing function for formatting. It uses the internal format logic to substitute placeholders with argument values.
	 *
	 * @tparam Ts The types of the arguments to format.
	 *
	 * @param fmt The format string containing placeholders like {}.
	 * @param args The arguments to substitute into the placeholders.
	 *
	 * @return The fully formatted string result.
	 */
	template<typename... Ts>
	ASTRA_API std::string format(std::string_view fmt, Ts&&... args) {
		std::string result;
		format(&result, fmt, 0, std::forward<Ts>(args)...);
		return result;
	}

}