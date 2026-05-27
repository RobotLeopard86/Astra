#pragma once

#include <type_traits>

#include "types/type_actions.hpp"// IWYU pragma: keep
#include "type_id.hpp"
#include "astra/dll.hpp"

class ASTRA_API AstraReflectBase {
  public:
	virtual astra::TypeId ASTRA__gettypeid() const = 0;
};

#ifdef _ASTRAGENERATE
template<typename T>
struct ASTRA_API astragen_reflectable_check {
	static constexpr bool value = std::is_enum_v<T> || (!std::is_class_v<T>) || (std::is_class_v<T> && std::default_initializable<T> && std::is_base_of_v<AstraReflectBase, T>);
};
#define ASTRA_REFLECT [[clang::annotate("astra.reflect")]]
#define ASTRA_IGNORE [[clang::annotate("astra.ignore")]]
#define ASTRA_ALIAS(x) [[clang::annotate("astra.alias:" x)]]
#define ASTRASETUP(x)                                                                                                                       \
	friend struct astra::TypeActions<x>;                                                                                                    \
	ASTRA_IGNORE virtual astra::TypeId ASTRA__gettypeid() const override;                                                                   \
	ASTRA_IGNORE void forceRCInstantiate() {                                                                                                \
		static_assert(astragen_reflectable_check<x>::value, "Cannot reflect a type that does not meet the conditions for reflectability!"); \
	}
#else
#define ASTRA_REFLECT
#define ASTRA_IGNORE
#define ASTRA_ALIAS(x)
#define ASTRASETUP(x)                    \
	friend struct astra::TypeActions<x>; \
	virtual astra::TypeId ASTRA__gettypeid() const override;
#endif