#pragma once

#include <istream>
#include <string>

#include "dll.hpp"
#include "var.hpp"
#include "reflectable.hpp"

#include "yaml-cpp/yaml.h"// IWYU pragma: keep

namespace astra {
	/**
	 * @brief Helper for serializing and deserializing objects to/from YAML
	 */
	struct ASTRA_API yaml {
		/**
		 * @brief Deserialize a T object from the YAML node
		 *
		 * @tparam T The serializable type to deserialize to
		 *
		 * @param node The YAML node to deserialize from
		 *
		 * @return A T object storing the data from the document
		 */
		template<Serializable T>
		static T fromNode(const YAML::Node& node) {
			T obj;
			deserialize(Var(&obj), node);
			return obj;
		}

		///@cond
		template<Serializable T>
			requires(!is_reflectable_v<T>)
		static T fromNode(const YAML::Node& node) {
			using S = SerializedSubstitute<T>;
			S obj;
			deserialize(Var(&obj), node);
			return T {obj};
		}
		///@endcond

		/**
		 * @brief Deserialize a T object from a YAML string
		 *
		 * @tparam T The serializable type to deserialize to
		 *
		 * @param str A string storing YAML to deserialize from
		 *
		 * @return A T object storing the data from the YAML
		 */
		template<Serializable T>
		static T fromString(const std::string& str) {
			YAML::Node node = YAML::Load(str);
			return fromNode<T>(node);
		}

		/**
		 * @brief Deserialize a T object from a stream of YAML text
		 *
		 * @tparam T The serializable type to deserialize to
		 *
		 * @param stream A stream storing YAML to deserialize from
		 *
		 * @return A T object storing the data from the YAML
		 */
		template<Serializable T>
		static T fromStream(std::istream& stream) {
			YAML::Node node = YAML::Load(stream);
			return fromNode<T>(node);
		}

		/**
		 * @brief Deserialize a YAML node into a Var
		 *
		 * @param node The YAML node to deserialize from
		 * @param var The Var to write into
		 */
		static void fromNodeIntoVar(const YAML::Node& json, Var var) {
			deserialize(var, json);
		}

		/**
		 * @brief Deserialize a YAML string into a Var
		 *
		 * @param str A string storing YAML to deserialize from
		 * @param var The Var to write into
		 */
		static void fromStringIntoVar(const std::string& str, Var var) {
			YAML::Node node = YAML::Load(str);
			return fromNodeIntoVar(node, var);
		}

		/**
		 * @brief Deserialize a stream of YAML text into a Var
		 *
		 * @param stream A stream storing YAML to deserialize from
		 * @param var The Var to write into
		 */
		static void fromStreamIntoVar(std::istream& stream, Var var) {
			YAML::Node node = YAML::Load(stream);
			return fromNodeIntoVar(node, var);
		}

		/**
		 * @brief Serialize a T object to a YAML node
		 *
		 * @tparam T The serializable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A YAML node containing the serialized data
		 */
		template<Serializable T>
		static YAML::Node toNode(const T* obj) {
			YAML::Node node;
			node["__astraforcemapcreate__"] = true;
			node.remove("__astraforcemapcreate__");
			serialize(node, Var(obj));
			return node;
		}

		///@cond
		template<Serializable T>
			requires(!is_reflectable_v<T>)
		static YAML::Node toNode(const T* obj) {
			using S = SerializedSubstitute<T>;
			YAML::Node node;
			node["__astraforcemapcreate__"] = true;
			node.remove("__astraforcemapcreate__");
			S sub = obj->ASTRA__getserialized();
			serialize(node, Var(&sub));
			return node;
		}
		///@endcond

		/**
		 * @brief Serialize a T object to a YAML string
		 *
		 * @tparam T The serializable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A vector containing the serialized data encoded in YAML
		 */
		template<Serializable T>
		static std::string toString(const T* obj) {
			YAML::Emitter em;
			em << toNode<T>(obj);
			return em.c_str();
		}

		/**
		 * @brief Serialize a T object to a stream as YAML
		 *
		 * @tparam T The serializable type to serialize from
		 *
		 * @param stream The stream to write the serialized YAML to
		 * @param obj The object to serialize
		 */
		template<Serializable T>
		static void toStream(std::ostream& stream, const T* obj) {
			stream << toString<T>(obj);
		}

		/**
		 * @brief Serialize a Var to a YAML node
		 *
		 * @param var A Var holding the object to serialize
		 *
		 * @return A YAML node containing the serialized data
		 */
		static YAML::Node toNodeFromVar(Var var) {
			YAML::Node node;
			node["__astraforcemapcreate__"] = true;
			node.remove("__astraforcemapcreate__");
			serialize(node, var);
			return node;
		}

		/**
		 * @brief Serialize a Var to a YAML string
		 *
		 * @param var A Var holding the object to serialize
		 *
		 * @return A vector containing the serialized data encoded in YAML
		 */
		static std::string toStringFromVar(Var var) {
			YAML::Emitter em;
			em << toNodeFromVar(var);
			return em.c_str();
		}

		/**
		 * @brief Serialize a Var to a stream as YAML
		 *
		 * @param stream The stream to write the serialized YAML to
		 * @param var A Var holding the object to serialize
		 */
		static void toStreamFromVar(std::ostream& stream, Var var) {
			stream << toNodeFromVar(var);
		}

	  private:
		static void serialize(YAML::Node& node, Var var);
		static void deserialize(Var var, const YAML::Node& node);
	};
}