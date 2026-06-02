R"(
#pragma once

// IWYU pragma: private; include "{{project}}.astra.hpp"

#include "astra/type_id.hpp"
#include "astra/type_actions/type_actions.hpp"

#include "{{origin}}"

namespace astra {

template<>
struct TypeActions<{{name}}>;

template<>
TypeId TypeId::get<{{name}}>();

}
)"