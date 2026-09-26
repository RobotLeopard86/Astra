R"(
{% if (namespace != "") -%}
using namespace {{namespace}};
{% endif -%}

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

inline TypeId TypeIdFactory<{{name}}>::get() {
  static TypeId id(TypeTable::recordActions(Actions(&TypeActions<{{name}}>::reflect,
                                                 &CommonActions<{{name}}>::typeName,
                                                 &CommonActions<{{name}}>::sizeOf,
                                                 &CommonActions<{{name}}>::construct,
                                                 &CommonActions<{{name}}>::destroy,
                                                 &CommonActions<{{name}}>::copy,
                                                 &CommonActions<{{name}}>::move)));
  return id;
}
)"