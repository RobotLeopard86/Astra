R"(
#include "{{file_name}}"

#include "astra/actions_table.hpp"
#include "astra/type_actions/common_actions.hpp"
#include "astra/type_actions/type_actions.hpp"

namespace astra {

template<>
struct TypeActions<{{name}}> {
  static TypeInfo reflect(void* value, bool isConst) {
    auto* p = static_cast<{{name}}*>(value);

    static std::map<std::string_view, FieldDesc> fields {% if length(fields) != 0 %}{
      {% else %}{};{% endif -%}
      {% for item in fields -%}
        {"{{item.alias}}", FieldDesc(&{{name}}::{{item.name}}, Access::{{join(item.acc," | Access::")}})},
    {% if not loop.is_last %}  {% else %}};{% endif -%}
      {% endfor %}
    static std::map<std::string_view, MethodDesc> methods {% if length(methods) != 0 %}{
      {% else %}{};{% endif -%}
      {% for item in methods -%}
        {"{{item.alias}}", MethodDesc(&invoke_{{item.safe_name}}, Access::{{join(item.acc," | Access::")}})},
    {% if not loop.is_last %}  {% else %}};{% endif -%}
      {% endfor %}
    return Object(Var(p, isConst), &fields, &methods);
  }
{% for item in methods %}
  static void invoke_{{item.safe_name}}(Var res, void* obj, const std::vector<Var>& args) {
    if (args.size() != {{length(item.params)}}) {
      throw std::runtime_error("Number of arguments is not the same as in the callable");
    }

    {% for type in item.params -%}
    if (args[{{loop.index}}].typeId() != TypeId::get<{{type}}>()) {
      throw std::runtime_error(astra::format("Argument type expected: {{type}}, received: {}", typeName(args[{{loop.index}}].typeId())));
    }
    const {{type}}& a{{loop.index}} = *static_cast<const {{type}}*>(args[{{loop.index}}].raw());

    {% endfor -%}

    {% if item.return != "void" -%}
    if (res.typeId() != TypeId::get<{{item.return}}>()) {
      throw std::runtime_error(astra::format("Return type expected: {{item.return}}, received: {}", typeName(res.typeId())));
    }
    *reinterpret_cast<{{item.return}}*>(res.rawMut()) = 
    {% endif -%}
    std::invoke(&{{name}}::{{item.name}}{% if not ("Static" in item.acc) %}, static_cast<{{name}}*>(obj){% endif %}{% if length(item.params) != 0 %},{% else %});{% endif %}
      {% for i in range(length(item.params)) -%}
        a{{loop.index}}{% if not loop.is_last %},
      {% else %});{% endif -%}
      {% endfor %}
  }
{% endfor -%}
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

astra::TypeId {{name}}::ASTRA__gettypeid() const {
	return astra::TypeId::get<{{name}}>();
}
)"