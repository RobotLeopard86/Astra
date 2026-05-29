#pragma once

#include "nlohmann/json.hpp"
#include "yaml-cpp/yaml.h"// IWYU pragma: keep
#include "libjaguar/Document.hpp"

#include "dll.hpp"

namespace astra {
	struct ASTRA_API convert {
		static nlohmann::json yamlToJson(const YAML::Node& node);
		static nlohmann::json binaryToJson(libjaguar::Document& doc);
		static YAML::Node jsonToYaml(const nlohmann::json& json);
		static YAML::Node binaryToYaml(libjaguar::Document& doc);
		static libjaguar::Document jsonToBinary(const nlohmann::json& json);
		static libjaguar::Document yamlToBinary(const YAML::Node& node);
		static std::string yamlStringToJsonString(const std::string& str);
		static std::string yamlStreamToJsonString(std::istream& stream);
		static std::string binaryVecToJsonString(const std::vector<uint8_t>& vector);
		static std::string binaryStreamToJsonString(std::istream& stream);
		static std::string jsonStringToYamlString(const std::string& str);
		static std::string jsonStreamToYamlString(std::istream& stream);
		static std::string binaryVecToYamlString(const std::vector<uint8_t>& vector);
		static std::string binaryStreamToYamlString(std::istream& stream);
		static std::vector<uint8_t> jsonStringToBinaryVec(const std::string& str);
		static std::vector<uint8_t> jsonStreamToBinaryVec(std::istream& stream);
		static std::vector<uint8_t> yamlStringToBinaryVec(const std::string& str);
		static std::vector<uint8_t> yamlStreamToBinaryVec(std::istream& stream);
	};
}