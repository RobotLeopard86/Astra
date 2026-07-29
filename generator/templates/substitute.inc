R"(
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
		{% for field in fields %}
		static decltype({{original_type}}::{{field.name}}) de_{{field.name}}({{field.type}} {{field.name}}) {
		{% if field.is_pointer %}
			return {{field.rewrap_expr}};
		{% else if field.container_kind == "vector" %}
			decltype({{original_type}}::{{field.name}}) out;
			for(const auto& item : {{field.name}}) {
				out.push_back(item.deserialize());
			}
			return out;
		{% else if field.container_kind == "set" %}
			decltype({{original_type}}::{{field.name}}) out;
			for(const auto& item : {{field.name}}) {
				out.insert(item.deserialize());
			}
			return out;
		{% else if field.container_kind == "map" %}
			decltype({{original_type}}::{{field.name}}) out;
			for(const auto& [k, v] : {{field.name}}) {
				out[k.deserialize()] = v.deserialize();
			}
			return out;
		{% else if field.container_kind == "array" %}
			decltype({{original_type}}::{{field.name}}) out;
			for(size_t i = 0; i < {{field.name}}.size(); ++i) {
				out[i] = {{field.name}}[i].deserialize();
			}
			return out;
		{% else %}
		{% if field.is_substitute_type %}
			return {{field.name}}.deserialize();
		{% else %}
			return {{field.name}};
		{% endif %}
		{% endif %}
		}
		{% endfor %}
	};
//
    SerializedSubstitute(const {{original_type}}& original)
        : {% for field in fields %}
          {{field.name}}(
            {% if field.is_pointer %}
            {% if field.is_substitute_type %}
				original.{{field.name}} ? {{field.type}}({{field.deref_expr}}) : {{field.type}}()
			{% else %}
				original.{{field.name}} ? {{field.deref_expr}} : {{field.type}}()
			{% endif %}
            {% else %}
                original.{{field.name}}
            {% endif %}
          ){% if not loop.is_last %}, {% endif %}
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