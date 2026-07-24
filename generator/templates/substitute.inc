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
                original.{{field.safe_name}} ? *original.{{field.safe_name}} : {{field.type}}() 
            {% else if field.is_vector %}
                // Conversion for vectors would typically require a loop/transform
                // For simplicity in this template, we assume member-wise assignment is handled
                original.{{field.safe_name}}
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
            res.{{field.safe_name}} = std::make_unique<typename decltype({{field.safe_name}})::value_type>(
                {{field.safe_name}}.deserialize()
            );
        {% else if field.is_vector %}
            for(const auto& item : {{field.safe_name}}) {
                res.{{field.safe_name}}.push_back(item.deserialize());
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