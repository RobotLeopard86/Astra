R"(
template<>
struct astra::SerializedSubstitute<{{original_type}}> : public AstraReflectBase {
    {% for field in fields %}
    {{field.type}} {{field.safe_name}};
    {% endfor %}

    SerializedSubstitute() = default;
    SerializedSubstitute(const SerializedSubstitute&) = default;
    SerializedSubstitute(SerializedSubstitute&&) = default;
    SerializedSubstitute& operator=(const SerializedSubstitute&) = default;
    SerializedSubstitute& operator=(SerializedSubstitute&&) = default;

    SerializedSubstitute(const {{original_type}}& original)
        : {% for field in fields %}
          {{field.safe_name}}(
            {% if field.is_pointer %}
                original.{{field.safe_name}} ? {{field.deref_expr}} : {{field.type}}()
            {% else %}
                original.{{field.safe_name}}
            {% endif %}
          ){% if not loop.is_last %}, {% endif %}
          {% endfor %}
    {}

    {{original_type}} deserialize() const {
        {{original_type}} res;
        {% for field in fields %}
        {% if field.is_pointer %}
            res.{{field.safe_name}} = {{field.rewrap_expr}};
        {% else if field.container_kind == "vector" %}
            for(const auto& item : {{field.safe_name}}) {
                res.{{field.safe_name}}.push_back(item.deserialize());
            }
        {% else if field.container_kind == "set" %}
            for(const auto& item : {{field.safe_name}}) {
                res.{{field.safe_name}}.insert(item.deserialize());
            }
        {% else if field.container_kind == "map" %}
            for(const auto& [k, v] : {{field.safe_name}}) {
                res.{{field.safe_name}}[k.deserialize()] = v.deserialize();
            }
        {% else if field.container_kind == "array" %}
            for(size_t i = 0; i < {{field.safe_name}}.size(); ++i) {
                res.{{field.safe_name}}[i] = {{field.safe_name}}[i].deserialize();
            }
        {% else %}
            res.{{field.safe_name}} = {% if field.is_substitute_type %}
                {{field.safe_name}}.deserialize();
            {% else %}
                {{field.safe_name}};
            {% endif %}
        {% endif %}
        {% endfor %}
        return res;
    }

    ASTRASETUP(SerializedSubstitute)
    virtual ~SerializedSubstitute() {}
};
)"