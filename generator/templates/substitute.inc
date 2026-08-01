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
        {% for conv in converters %}
        static {{conv.originalType}} de_{{conv.name}}({{conv.substitutedType}} s) {
            {% if conv.kind == "pointer" %}
                return {{conv.rewrap_expr}};
            {% else if conv.kind == "vector" %}
                decltype({{conv.originalType}}) out;
                for(const auto& item : s) {
                    out.push_back(cvt::de_{{conv.name}}_item(item));
                }
                return out;
            {% else if conv.kind == "set" %}
                decltype({{conv.originalType}}) out;
                for(const auto& item : s) {
                    out.insert(cvt::de_{{conv.name}}_item(item));
                }
                return out;
            {% else if conv.kind == "map" %}
                decltype({{conv.originalType}}) out;
                for(const auto& [k, v] : s) {
                    out[cvt::de_{{conv.name}}_key(k)] = cvt::de_{{conv.name}}_val(v);
                }
                return out;
            {% else if conv.kind == "array" %}
                decltype({{conv.originalType}}) out;
                for(size_t i = 0; i < s.size(); ++i) {
                    out[i] = cvt::de_{{conv.name}}_item(s[i]);
                }
                return out;
            {% else %}
                {% if conv.is_substitute %}
                    return s.deserialize();
                {% else %}
                    return s;
                {% endif %}
            {% endif %}
        }
//
        static {{conv.substitutedType}} en_{{conv.name}}(const {{conv.originalType}}& t) {
            {% if conv.kind == "pointer" %}
                return t ? astra::SerializedSubstitute<{{conv.substitutedType}}>(*t) : {{conv.substitutedType}}();
            {% else if conv.kind == "vector" %}
                {{conv.substitutedType}} out;
                for(const auto& item : t) {
                    out.push_back(cvt::en_{{conv.name}}_item(item));
                }
                return out;
            {% else if conv.kind == "set" %}
                {{conv.substitutedType}} out;
                for(const auto& item : t) {
                    out.insert(cvt::en_{{conv.name}}_item(item));
                }
                return out;
            {% else if conv.kind == "map" %}
                {{conv.substitutedType}} out;
                for(const auto& [k, v] : t) {
                    out[cvt::en_{{conv.name}}_key(k)] = cvt::en_{{conv.name}}_val(v);
                }
                return out;
            {% else if conv.kind == "array" %}
                {{conv.substitutedType}} out;
                for(size_t i = 0; i < t.size(); ++i) {
                    out[i] = cvt::en_{{conv.name}}_item(t[i]);
                }
                return out;
            {% else %}
                return astra::SerializedSubstitute<{{conv.substitutedType}}>(t);
            {% endif %}
        }
        {% endfor %}
    };
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
};
)"