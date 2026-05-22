#pragma once

#include <concepts>// IWYU pragma: keep

#include "setup.hpp"

namespace astra {
	template<typename T>
	concept Reflectable = std::is_enum_v<T> || (!std::is_class_v<T>) || (std::is_class_v<T> && std::default_initializable<T> && std::is_base_of_v<AstraReflectBase, T>);
}