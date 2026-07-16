#pragma once

#include <concepts>// IWYU pragma: keep
#include <type_traits>

#include "dll.hpp"
#include "serialized_substitute.hpp"

///@cond
class ASTRA_API AstraReflectBase;

namespace astra {
	template<typename T, typename = void>
	struct is_substitute_valid : std::true_type {};

	template<typename T>
	struct is_reflectable : public std::false_type {};

	template<typename T>
	struct is_serializable : public std::false_type {};

	template<typename T>
	struct is_substitute_valid<T, std::void_t<decltype(SerializedSubstitute<T>::invalid)>> {
		static constexpr bool value = !SerializedSubstitute<T>::invalid;
	};

	template<typename T>
	constexpr inline bool has_serialized_v = is_substitute_valid<T>::value;

	template<typename T>
		requires std::is_enum_v<T> || (!std::is_class_v<T>) || (std::is_class_v<T> && std::default_initializable<T> && std::is_base_of_v<AstraReflectBase, T>)
	struct is_reflectable<T> : public std::true_type {};

	template<typename T>
	constexpr inline bool is_reflectable_v = is_reflectable<T>::value;

	template<typename T>
	concept can_get_serializable = has_serialized_v<T> && requires(const T& obj) {
		{ obj.ASTRA__getserialized() } noexcept -> std::same_as<astra::SerializedSubstitute<T>>;
	};

	template<typename T>
		requires is_reflectable_v<T> || (std::is_class_v<T> && has_serialized_v<T> && is_reflectable_v<SerializedSubstitute<T>> && std::constructible_from<T, SerializedSubstitute<T>> && can_get_serializable<T>)
	struct is_serializable<T> : public std::true_type {};

	template<typename T>
	constexpr inline bool is_serializable_v = is_serializable<T>::value;

	template<typename T>
	concept Reflectable = is_reflectable_v<T>;

	template<typename T>
	concept Serializable = is_serializable_v<T>;
}
///@endcond