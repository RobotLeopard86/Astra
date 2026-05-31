#pragma once

#include <cstddef>
#include <filesystem>
#include <iomanip>
#include <string>
#include <type_traits>

#include "append_buf.hpp"
#include "traits.hpp"
#include "dll.hpp"

namespace astra {

	/**
	 * @brief Appends a value of type T to a target std::string.
	 * @details This overload is used when the type T is itself a string.
	 *
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The string value to append.
	 */
	template<typename T>
		requires is_string_v<std::remove_cvref_t<T>>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg;
	}

	/**
	 * @brief Appends a value of type T to a target std::string.
	 * @details This overload is used for appending C-style arrays (e.g., character arrays).
	 *
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The array content to append.
	 */
	template<typename T>
		requires std::is_array_v<std::remove_cvref_t<T>> && std::is_same_v<array_value_t<T>, const char>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg;
	}

	/**
	 * @brief Appends an integral type to a target std::string.
	 * @details Handles integer types (excluding bool and char) by converting them to their string representation.
	 *
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The integral value to append.
	 */
	template<typename T>
		requires std::is_integral_v<std::remove_cvref_t<T>> && (!std::is_same_v<std::remove_cvref_t<T>, bool>) && (!std::is_same_v<std::remove_cvref_t<T>, char>)
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += std::to_string(arg);
	}

	/**
	 * @brief Appends a floating-point number to a target std::string.
	 * @details Formats the float/double to two decimal places before appending.
	 *
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The floating-point value to append.
	 */
	template<typename T>
		requires std::is_floating_point_v<std::remove_cvref_t<T>>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		AppendBuf buf(str);
		std::ostream stream(&buf);
		stream << std::setiosflags(std::ios::fixed) << std::setprecision(2);
		stream << arg;
	}

	/**
	 * @brief Appends a boolean value to a target std::string.
	 * @details Appends the literal string "true" or "false" based on the boolean value.
	 *
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The boolean value to append.
	 */
	template<typename T>
		requires std::is_same_v<std::remove_cvref_t<T>, bool>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg ? "true" : "false";
	}

	/**
	 * @brief Appends a single character to a target std::string.
	 * @details Overloaded specifically for char type.
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The character to append.
	 */
	template<typename T>
		requires std::is_same_v<std::remove_cvref_t<T>, char>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg;
	}

	/**
	 * @brief Appends a std::filesystem::path object to a target std::string.
	 * @details Converts the path object to its string representation before appending.
	 * @param str Pointer to the target std::string that will receive the appended data.
	 * @param arg The path object to append.
	 */
	template<typename T>
		requires std::is_same_v<std::remove_cvref_t<T>, std::filesystem::path>
	ASTRA_API inline void append(std::string* str, T&& arg) {
		*str += arg.string();
	}

	/**
	 * @brief Copies a substring of the format string.
	 * @details Used internally for recursive format string processing.
	 *
	 * @param result Pointer to the resulting string buffer.
	 * @param fmt The full format string view.
	 * @param i The starting index for the substring.
	 */
	[[maybe_unused]] inline void format(std::string* result, std::string_view fmt, std::size_t i) {
		*result += fmt.substr(i, fmt.size() - i);
	}

	/**
	 * @brief Recursively processes the format string, appending arguments as placeholders are found.
	 * @details This function traverses the format string, appending literal text segments and recursively calling itself or `append` for arguments found inside placeholders `{}`.
	 *
	 * @param result Pointer to the resulting string buffer.
	 * @param fmt The format string view containing placeholders.
	 * @param i The starting index in the format string.
	 * @param arg The first argument to format.
	 * @param args The remaining arguments for formatting.
	 */
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