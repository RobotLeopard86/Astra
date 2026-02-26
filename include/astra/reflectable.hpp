#pragma once

#include <concepts>
#include <type_traits>

#include "astra/type_id.hpp"

namespace astra {
	template<typename T>
	concept Reflectable = std::is_enum_v<T> || (!std::is_class_v<T>) || (std::is_class_v<T> && std::default_initializable<T> && requires(const T t) {
		{ t.ASTRA__gettypeid() } -> std::same_as<TypeId>;
	});
}