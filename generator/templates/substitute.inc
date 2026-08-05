R"(

{% for header in headers -%}
#include "astra_generated/{{header}}.astra.hpp"
{% endfor -%}

template<>
struct astra::SerializedSubstitute<{{original_type}}> : public AstraReflectBase {
    {% for field in fields -%}
    {{field.type}} {{field.name}};
    {% endfor %}

    SerializedSubstitute() = default;
    SerializedSubstitute(const SerializedSubstitute&) = default;
    SerializedSubstitute(SerializedSubstitute&&) = default;
    SerializedSubstitute& operator=(const SerializedSubstitute&) = default;
    SerializedSubstitute& operator=(SerializedSubstitute&&) = default;

    struct cvt {
        {% for cvt in converters -%}
        static {{ cvt.original_type }} de_{{ cvt.fn_name }}({{ cvt.serialized_type }} s0) {
            {% for step in reverse(cvt.steps) -%}
            // {{ step.op }}
            {{ step.original_type }} s{{ loop.index1 }} =
            {% if step.op == "direct" -%}
                s{{ loop.index1 - 1}};
            {% else if step.op == "sub" -%}
                s{{ loop.index1 - 1}}.deserialize();
            {% else if step.op == "unique" -%}
                std::make_unique<{{ step.serialized_type }}>(s{{ loop.index1 - 1}});
            {% else if step.op == "shared" -%}
                std::make_shared<{{ step.serialized_type }}>(s{{ loop.index1 - 1}});
            {% else if step.op == "raw" -%}
                new {{ step.serialized_type }}(s{{ loop.index1 - 1}});
            {% else if step.op == "stlcvt" -%}
                {% if step.original_type == "std::string" -%}
                s{{ loop.index1 - 1}};
                {% else if step.original_type == "std::string_view" -%}
                std::string_view(s{{ loop.index1 - 1}});
                {% else if step.original_type == "const char*" -%}
                s{{ loop.index1 - 1}}.c_str();
                {% else if step.original_type == "std::size_t" -%}
                std::size_t(s{{ loop.index1 - 1}});
                {% else if step.original_type == "std::byte" -%}
                std::byte(s{{ loop.index1 - 1}});
                {% else -%}
                {% if startsWith(step.original_type, "std::set") -%}
                {{ step.original_type }}(s{{ loop.index1 - 1}});
                {% else -%}
                {{ step.original_type }}();
                for(std::size_t i = 0; i < s{{ loop.index1 - 1}}.size(); ++i) {
                    s{{ loop.index1 }}.push(s{{ loop.index1 - 1}}[i]);
                }
                {% endif -%}
                {% endif -%}
            {% else if step.op == "list" -%}
                {{ step.original_type }}();
                for(std::size_t i = 0; i < s{{ loop.index1 - 1}}.size(); ++i) {
                    {% if not startsWith(step.original_type, "std::array") -%}
                    s{{ loop.index1 }}.push_back(de_{{ step.item_cvt }}(s{{ loop.index1 - 1}}[i]));
                    {% else -%}
                    s{{ loop.index1 }}[i] = de_{{ step.item_cvt }}(s{{ loop.index1 - 1}}[i]);
                    {% endif -%}
                }
            {% else if step.op == "map" -%}
                {{ step.original_type }}();
                for(const auto& [k, v] : s{{ loop.index1 - 1 }}) {
                    s{{ loop.index1 }}[de_{{ step.key_cvt }}(k)] = de_{{ step.val_cvt }}(v);
                }
            {% endif -%}

            {% if loop.is_last -%} return s{{ loop.index1 }}; {% endif -%}
            {% endfor %}
        }

        static {{ cvt.serialized_type }} en_{{ cvt.fn_name }}(const {{ cvt.original_type }}& s0) {
            {% for step in cvt.steps -%}
            // {{ step.op }}
            {{ step.serialized_type }} s{{ loop.index1 }} =
            {% if step.op == "direct" -%}
                s{{ loop.index1 - 1}};
            {% else if step.op == "sub" -%}
                {{ step.serialized_type }}(s{{ loop.index1 - 1}});
            {% else if step.op == "unique" -%}
                *s{{ loop.index1 - 1}};
            {% else if step.op == "shared" -%}
                *s{{ loop.index1 - 1}};
            {% else if step.op == "raw" -%}
                *s{{ loop.index1 - 1}};
            {% else if step.op == "stlcvt" -%}
                {% if not step.container -%}
                {{ step.serialized_type }}(s{{ loop.index1 - 1}});
                {% else -%}
                {% if startsWith(step.original_type, "std::set") -%}
                {{ step.serialized_type }}(s{{ loop.index1 - 1}});
                {% else -%}
                {{ step.serialized_type }}();
                {
                    auto container = s{{ loop.index1 - 1}};
                    for(std::size_t i = 0; i < container.size(); ++i) {
                        auto item = container.{% if startsWith(step.original_type, "std::queue") -%}front{% else -%}top{% endif -%}();
                        container.pop();
                        {% if not startsWith(step.serialized_type, "std::array") -%}
                        s{{ loop.index1 }}.push_back(item);
                        {% else -%}
                        s{{ loop.index1 }}[i] = item;
                        {% endif -%}
                    }
                }
                {% endif -%}
                {% endif -%}
            {% else if step.op == "list" -%}
                {{ step.serialized_type }}();
                for(std::size_t i = 0; i < s{{ loop.index1 - 1}}.size(); ++i) {
                    {% if not startsWith(step.serialized_type, "std::array") -%}
                    s{{ loop.index1 }}.push_back(en_{{ step.item_cvt }}(s{{ loop.index1 - 1}}[i]));
                    {% else -%}
                    s{{ loop.index1 }}[i] = en_{{ step.item_cvt }}(s{{ loop.index1 - 1}}[i]);
                    {% endif -%}
                }
            {% else if step.op == "map" -%}
                {{ step.serialized_type }}();
                for(const auto& [k, v] : s{{ loop.index1 - 1 }}) {
                    s{{ loop.index1 }}[en_{{ step.key_cvt }}(k)] = en_{{ step.val_cvt }}(v);
                }
            {% endif -%}

            {% if loop.is_last -%} return s{{ loop.index1 }}; {% endif -%}
            {% endfor %}
        }

        {% endfor -%}
    };

    SerializedSubstitute(const {{original_type}}& original)
        : {% for field in fields -%}
          {{field.name}}(cvt::en_{{field.name}}(original.{{field.name}})){% if not loop.is_last -%}, {% endif -%}
          {% endfor -%}
    {}

    {{original_type}} deserialize() const {
        {{original_type}} res;
        {% for field in fields -%}
        res.{{field.name}} = cvt::de_{{field.name}}({{field.name}});
        {% endfor -%}
        return res;
    }

    ASTRASETUP(SerializedSubstitute)
    virtual ~SerializedSubstitute() {}
};
)"