#pragma once

#include <istream>
#include <string>

#include "dll.hpp"
#include "var.hpp"
#include "reflectable.hpp"

#include "yaml-cpp/yaml.h"// IWYU pragma: keep

namespace astra {
	struct ASTRA_API yaml {
		template<Reflectable T>
		static T fromNode(const YAML::Node& node) {
			T obj;
			deserialize(Var(&obj), node);
			return obj;
		}

		template<Reflectable T>
		static T fromString(const std::string& str) {
			YAML::Node node = YAML::Load(str);
			return fromNode<T>(node);
		}

		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			YAML::Node node = YAML::Load(stream);
			return fromNode<T>(node);
		}

		template<Reflectable T>
		static YAML::Node toNode(const T* obj) {
			YAML::Node node;
			node["__astraforcemapcreate__"] = true;
			node.remove("__astraforcemapcreate__");
			serialize(node, Var(obj));
			return node;
		}

		template<Reflectable T>
		static std::string toString(const T* obj) {
			YAML::Emitter em;
			em << toNode<T>(obj);
			return em.c_str();
		}

		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			stream << toString<T>(obj);
		}

	  private:
		static void serialize(YAML::Node& node, Var var);
		static void deserialize(Var var, const YAML::Node& node);
	};
}