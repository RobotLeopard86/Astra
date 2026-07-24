#pragma once

#include "type_actions/type_actions.hpp"// IWYU pragma: keep
#include "type_id.hpp"
#include "dll.hpp"
#include "reflectable.hpp"

///@cond
class ASTRA_API AstraReflectBase {
  public:
	virtual astra::TypeId ASTRA__gettypeid() const = 0;
};

#ifdef _ASTRAGENERATE
#define ASTRA_REFLECT [[clang::annotate("astra.reflect")]]
#define ASTRA_IGNORE [[clang::annotate("astra.ignore")]]
#define ASTRA_ALIAS(x) [[clang::annotate("astra.alias:" x)]]
#define ASTRASETUP(x)                                                                                                                                  \
	friend struct astra::TypeActions<x>;                                                                                                               \
	friend struct astra::SerializedSubstitute<x>;                                                                                                      \
	ASTRA_IGNORE virtual astra::TypeId ASTRA__gettypeid() const override;                                                                              \
	ASTRA_IGNORE void ASTRA__checkreflectability() {                                                                                                   \
		static_assert(astra::is_reflectable_v<x>, "Cannot generate reflection data for a type that does not meet the conditions for reflectability!"); \
	}
#else
#define ASTRA_REFLECT
#define ASTRA_IGNORE
#define ASTRA_ALIAS(x)
#define ASTRASETUP(x)                             \
	friend struct astra::TypeActions<x>;          \
	friend struct astra::SerializedSubstitute<x>; \
	virtual astra::TypeId ASTRA__gettypeid() const override;
#endif
///@endcond