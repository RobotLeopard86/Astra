R"(
#include "{{file_name}}"

// clang-format off
#include "astra/actions_table.hpp"
#include "astra/type_actions/common_actions.hpp"
#include "astra/type_actions/type_actions.hpp"

{% if (namespace != "") -%}
using namespace {{namespace}};
{% endif -%}

namespace astra {

template<>
struct TypeActions<{{name}}> {
  static TypeInfo reflect(void* value, bool isConst) {

    static constexpr ConstexprMap<{{name}}, {{length(constants)}}> constants {% if length(constants) != 0 -%}{{"{{{"}}
      {% else -%}{};{% endif -%}
      {% for item in constants -%}
      {{"{"}}{{name}}::{{item.name}}, "{{item.alias}}"},
    {% if not loop.is_last -%}  {% else -%}}}};{% endif -%}
      {% endfor %}
    return Enum(static_cast<{{name}}*>(value), isConst, constants);
  }
};

template<>
TypeId TypeId::get<{{name}}>() {
  static TypeId id(ActionsTable::record(Actions(&TypeActions<{{name}}>::reflect,      
                                                 &CommonActions<{{name}}>::typeName,  
                                                 &CommonActions<{{name}}>::sizeOf,  
                                                 &CommonActions<{{name}}>::construct,  
                                                 &CommonActions<{{name}}>::destroy,    
                                                 &CommonActions<{{name}}>::copy,       
                                                 &CommonActions<{{name}}>::move)));
  return id;
}

}

// clang-format on
)"