#pragma once

#include <istream>
#include <string>

#include "dll.hpp"
#include "var.hpp"
#include "reflectable.hpp"

#include "yaml-cpp/yaml.h"// IWYU pragma: keep

namespace astra {
	struct ASTRA_API yaml {
		/**
		 * @brief Deserialize a T object from the YAML node
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param node The YAML node to deserialize from
		 *
		 * @return A T object storing the data from the document
		 */
		template<Reflectable T>
		static T fromNode(const YAML::Node& node) {
			T obj;
			deserialize(Var(&obj), node);
			return obj;
		}

		/**
		 * @brief Deserialize a T object from a YAML string
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param str A string storing YAML to deserialize from
		 *
		 * @return A T object storing the data from the YAML
		 */
		template<Reflectable T>
		static T fromString(const std::string& str) {
			YAML::Node node = YAML::Load(str);
			return fromNode<T>(node);
		}

		/**
		 * @brief Deserialize a T object from a stream of YAML text
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param stream A stream storing YAML to deserialize from
		 *
		 * @return A T object storing the data from the YAML
		 */
		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			YAML::Node node = YAML::Load(stream);
			return fromNode<T>(node);
		}

		/**
		 * @brief Serialize a T object to a YAML node
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A YAML node containing the serialized data
		 */
		template<Reflectable T>
		static YAML::Node toNode(const T* obj) {
			YAML::Node node;
			node["__astraforcemapcreate__"] = true;
			node.remove("__astraforcemapcreate__");
			serialize(node, Var(obj));
			return node;
		}

		/**
		 * @brief Serialize a T object to a YAML string
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A vector containing the serialized data encoded in YAML
		 */
		template<Reflectable T>
		static std::string toString(const T* obj) {
			YAML::Emitter em;
			em << toNode<T>(obj);
			return em.c_str();
		}

		/**
		 * @brief Serialize a T object to a stream as YAML
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param stream The stream to write the serialized YAML to
		 * @param obj The object to serialize
		 */
		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			stream << toString<T>(obj);
		}

	  private:
		static void serialize(YAML::Node& node, Var var);
		static void deserialize(Var var, const YAML::Node& node);
	};
}