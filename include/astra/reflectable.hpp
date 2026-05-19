#pragma once

#include <concepts>
#include <type_traits>

#include "types/type_actions.hpp"// IWYU pragma: keep
#include "type_id.hpp"			 // IWYU pragma: keep

#ifdef _ASTRAGENERATE
#define ASTRA_REFLECT [[clang::annotate("astra.reflect")]]
#define ASTRA_IGNORE [[clang::annotate("astra.ignore")]]
#define ASTRA_ALIAS(x) [[clang::annotate("astra.alias." x)]]
#define ASTRASETUP(x)                                        \
	friend struct astra::TypeActions<x>;                     \
	virtual astra::TypeId ASTRA__gettypeid() const override; \
	template struct astragen_reflectable_check<x>;
#else
#define ASTRA_REFLECT
#define ASTRA_IGNORE
#define ASTRA_ALIAS(x)
#define ASTRASETUP(x)                    \
	friend struct astra::TypeActions<x>; \
	virtual astra::TypeId ASTRA__gettypeid() const override;
#endif

class AstraReflectBase {
  public:
	virtual astra::TypeId ASTRA__gettypeid() const {
		return astra::TypeId::get<astra::None>();
	}
};

namespace astra {
	template<typename T>
	concept Reflectable = std::is_enum_v<T> || (!std::is_class_v<T>) || (std::is_class_v<T> && std::default_initializable<T> && std::is_base_of_v<AstraReflectBase, T>);
}

#ifdef _ASTRAGENERATE
template<typename T>
struct astragen_reflectable_check {
	static constexpr bool value = astra::Reflectable<T>;
};
#endif