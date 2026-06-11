#pragma once

#include <istream>

#include "dll.hpp"
#include "var.hpp"
#include "reflectable.hpp"

#include "nlohmann/json.hpp"

namespace astra {
	/**
	 * @brief Helper for serializing and deserializing objects to/from JSON
	 */
	struct ASTRA_API json {
		/**
		 * @brief Deserialize a T object from the JSON object
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param json The JSON object to deserialize from
		 *
		 * @return A T object storing the data from the document
		 */
		template<Reflectable T>
		static T fromJson(const nlohmann::json& json) {
			T obj;
			deserialize(Var(&obj), json);
			return obj;
		}

		/**
		 * @brief Deserialize a T object from a JSON string
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param str A string storing JSON to deserialize from
		 *
		 * @return A T object storing the data from the JSON
		 */
		template<Reflectable T>
		static T fromString(const std::string& str) {
			return fromJson<T>(nlohmann::json::parse(str));
		}

		/**
		 * @brief Deserialize a T object from a stream of JSON text
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param stream A stream storing JSON to deserialize from
		 *
		 * @return A T object storing the data from the JSON
		 */
		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			nlohmann::json j;
			stream >> j;
			return fromJson<T>(j);
		}

		/**
		 * @brief Deserialize a JSON object into a Var
		 *
		 * @param json The JSON object to deserialize from
		 * @param var The Var to write into
		 */
		static void fromJsonIntoVar(const nlohmann::json& json, Var var) {
			deserialize(var, json);
		}

		/**
		 * @brief Deserialize a JSON string into a Var
		 *
		 * @param str A string storing JSON to deserialize from
		 * @param var The Var to write into
		 */
		static void fromStringIntoVar(const std::string& str, Var var) {
			return fromJsonIntoVar(nlohmann::json::parse(str), var);
		}

		/**
		 * @brief Deserialize a stream of JSON text into a Var
		 *
		 * @param stream A stream storing JSON to deserialize from
		 * @param var The Var to write into
		 */
		static void fromStreamIntoVar(std::istream& stream, Var var) {
			nlohmann::json j;
			stream >> j;
			return fromJsonIntoVar(j, var);
		}

		/**
		 * @brief Serialize a T object to a JSON object
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A JSON object containing the serialized data
		 */
		template<Reflectable T>
		static nlohmann::json toJSON(const T* obj) {
			nlohmann::json j;
			serialize(j, Var(obj));
			return j;
		}

		/**
		 * @brief Serialize a T object to a JSON string
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A vector containing the serialized data encoded in JSON
		 */
		template<Reflectable T>
		static std::string toString(const T* obj) {
			return toJSON<T>(obj).dump();
		}

		/**
		 * @brief Serialize a T object to a stream as JSON
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param stream The stream to write the serialized JSON to
		 * @param obj The object to serialize
		 */
		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			stream << toJSON<T>(obj);
		}

		/**
		 * @brief Serialize a Var to a JSON object
		 *
		 * @param var A Var holding the object to serialize
		 *
		 * @return A JSON object containing the serialized data
		 */
		static nlohmann::json toJSONFromVar(Var var) {
			nlohmann::json j;
			serialize(j, var);
			return j;
		}

		/**
		 * @brief Serialize a Var to a JSON string
		 *
		 * @param var A Var holding the object to serialize
		 *
		 * @return A vector containing the serialized data encoded in JSON
		 */
		static std::string toStringFromVar(Var var) {
			return toJSONFromVar(var).dump();
		}

		/**
		 * @brief Serialize a Var to a stream as JSON
		 *
		 * @param stream The stream to write the serialized JSON to
		 * @param var A Var holding the object to serialize
		 */
		static void toStreamFromVar(std::ostream& stream, Var var) {
			stream << toJSONFromVar(var);
		}

	  private:
		static void serialize(nlohmann::json& json, Var var);
		static void deserialize(Var var, const nlohmann::json& json);
	};
}