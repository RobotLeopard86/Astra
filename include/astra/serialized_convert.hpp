#pragma once

#include "nlohmann/json.hpp"
#include "yaml-cpp/yaml.h"// IWYU pragma: keep
#include "libjaguar/Document.hpp"

#include "dll.hpp"

namespace astra {
	/**
	 * @brief Helper for performing in-place serialized format conversion
	 */
	struct ASTRA_API convert {
		/**
		 * @brief Convert a YAML node to a JSON object
		 *
		 * @param node The node to convert
		 *
		 * @return The resulting JSON
		 */
		static nlohmann::json yamlToJson(const YAML::Node& node);

		/**
		 * @brief Convert a Jaguar document to a JSON object
		 *
		 * @param doc The document to convert
		 *
		 * @return The resulting JSON
		 */
		static nlohmann::json binaryToJson(libjaguar::Document& doc);

		/**
		 * @brief Convert a JSON object to a YAML node
		 *
		 * @param json The JSON to convert
		 *
		 * @return The resulting node
		 */
		static YAML::Node jsonToYaml(const nlohmann::json& json);

		/**
		 * @brief Convert a Jaguar document to a YAML node
		 *
		 * @param doc The document to convert
		 *
		 * @return The resulting node
		 */
		static YAML::Node binaryToYaml(libjaguar::Document& doc);

		/**
		 * @brief Convert a JSON object to a Jaguar document
		 *
		 * @param json The JSON to convert
		 *
		 * @return The resulting document
		 */
		static libjaguar::Document jsonToBinary(const nlohmann::json& json);

		/**
		 * @brief Convert a YAML node to a Jaguar document
		 *
		 * @param node The node to convert
		 *
		 * @return The resulting document
		 */
		static libjaguar::Document yamlToBinary(const YAML::Node& node);

		/**
		 * @brief Convert a YAML string to a JSON string
		 *
		 * @param str The YAML string to convert
		 *
		 * @return The resulting JSON string
		 */
		static std::string yamlStringToJsonString(const std::string& str);

		/**
		 * @brief Convert a YAML stream to a JSON string
		 *
		 * @param stream The YAML stream to convert
		 *
		 * @return The resulting JSON string
		 */
		static std::string yamlStreamToJsonString(std::istream& stream);

		/**
		 * @brief Convert a Jaguar binary vector to a JSON string
		 *
		 * @param vector The binary vector to convert
		 *
		 * @return The resulting JSON string
		 */
		static std::string binaryVecToJsonString(const std::vector<uint8_t>& vector);

		/**
		 * @brief Convert a Jaguar binary stream to a JSON string
		 *
		 * @param stream The binary stream to convert
		 *
		 * @return The resulting JSON string
		 */
		static std::string binaryStreamToJsonString(std::istream& stream);

		/**
		 * @brief Convert a JSON string to a YAML string
		 *
		 * @param str The JSON string to convert
		 *
		 * @return The resulting YAML string
		 */
		static std::string jsonStringToYamlString(const std::string& str);

		/**
		 * @brief Convert a JSON stream to a YAML string
		 *
		 * @param stream The JSON stream to convert
		 *
		 * @return The resulting YAML string
		 */
		static std::string jsonStreamToYamlString(std::istream& stream);

		/**
		 * @brief Convert a Jaguar binary vector to a YAML string
		 *
		 * @param vector The binary vector to convert
		 *
		 * @return The resulting YAML string
		 */
		static std::string binaryVecToYamlString(const std::vector<uint8_t>& vector);

		/**
		 * @brief Convert a Jaguar binary stream to a YAML string
		 *
		 * @param stream The binary stream to convert
		 *
		 * @return The resulting YAML string
		 */
		static std::string binaryStreamToYamlString(std::istream& stream);

		/**
		 * @brief Convert a JSON string to a Jaguar binary vector
		 *
		 * @param str The JSON string to convert
		 *
		 * @return The resulting binary vector
		 */
		static std::vector<uint8_t> jsonStringToBinaryVec(const std::string& str);

		/**
		 * @brief Convert a JSON stream to a Jaguar binary vector
		 *
		 * @param stream The JSON stream to convert
		 *
		 * @return The resulting binary vector
		 */
		static std::vector<uint8_t> jsonStreamToBinaryVec(std::istream& stream);

		/**
		 * @brief Convert a YAML string to a Jaguar binary vector
		 *
		 * @param str The YAML string to convert
		 *
		 * @return The resulting binary vector
		 */
		static std::vector<uint8_t> yamlStringToBinaryVec(const std::string& str);

		/**
		 * @brief Convert a YAML stream to a Jaguar binary vector
		 *
		 * @param stream The YAML stream to convert
		 *
		 * @return The resulting binary vector
		 */
		static std::vector<uint8_t> yamlStreamToBinaryVec(std::istream& stream);
	};
}