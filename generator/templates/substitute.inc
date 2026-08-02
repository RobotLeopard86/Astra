R"(
//
{% for header in headers %}
#include "astra_generated/{{header}}.astra.hpp"
{% endfor %}
//
template<>
struct astra::SerializedSubstitute<{{original_type}}> : public AstraReflectBase {
    {% for field in fields %}
    {{field.type}} {{field.name}};
    {% endfor %}
//
    SerializedSubstitute() = default;
    SerializedSubstitute(const SerializedSubstitute&) = default;
    SerializedSubstitute(SerializedSubstitute&&) = default;
    SerializedSubstitute& operator=(const SerializedSubstitute&) = default;
    SerializedSubstitute& operator=(SerializedSubstitute&&) = default;
//
    struct cvt {
        {% for cvt in converters %}
        static {{ first(cvt.steps).original_type }} de_{{cvt.fn_name}}(auto s0) {
            {% for step in reverse(cvt.steps) %}
            // {{ step.op }}
            {% if loop.is_last %} return {% else %} auto s{{ loop.index1 }} = {% endif %}
            {% if step.op == "direct" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "sub" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "unique" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "shared" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "raw" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "stlcvt" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "list" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "map" %}
                s{{ loop.index1 - 1}};
            {% endif %}
            {% endfor %}
        }
//
        static auto en_{{cvt.fn_name}}({{ first(cvt.steps).original_type }} s0) {
            {% for step in reverse(cvt.steps) %}
            // {{ step.op }}
            {% if loop.is_last %} return {% else %} auto s{{ loop.index1 }} = {% endif %}
            {% if step.op == "direct" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "sub" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "unique" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "shared" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "raw" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "stlcvt" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "list" %}
                s{{ loop.index1 - 1}};
            {% else if step.op == "map" %}
                s{{ loop.index1 - 1}};
            {% endif %}
            {% endfor %}
        }
        {% endfor %}
    }
//
    SerializedSubstitute(const {{original_type}}& original)
        : {% for field in fields %}
          {{field.name}}(cvt::en_{{field.name}}(original.{{field.name}})){% if not loop.is_last %}, {% endif %}
          {% endfor %}
    {}
//
    {{original_type}} deserialize() const {
        {{original_type}} res;
        {% for field in fields %}
        res.{{field.name}} = cvt::de_{{field.name}}({{field.name}});
        {% endfor %}
        return res;
    }
//
    ASTRASETUP(SerializedSubstitute)
    virtual ~SerializedSubstitute() {}
)"