R"(
#pragma once

#include "astra/type_id.hpp"
#include "astra/types/type_actions.hpp"

// injected
#include "{{origin}}"

namespace astra {

template <>
struct TypeActions<{{name}}>;

template <>
TypeId TypeId::get({{name}}* /*unused*/);

}
)"