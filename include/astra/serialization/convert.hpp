#pragma once

#include "binary.hpp"
#include "json.hpp"
#include "yaml.hpp"

namespace astra::serialization {
	struct convert {   //NOLINT namespace like name
		struct toJson {//NOLINT namespace like name
			template<Reflectable T>
			static Expected<std::string> fromYamlString(std::string_view str) {
				Expected<T> yamlOut = yaml::fromString<T>(str);
				if(yamlOut.isError()) return Expected<std::string>(yamlOut.error());
				T obj = yamlOut.unwrap();
				return json::toString(&obj);
			}

			template<Reflectable T>
			static Expected<std::string> fromYamlStream(std::istream& stream) {
				Expected<T> yamlOut = yaml::fromString<T>(stream);
				if(yamlOut.isError()) return Expected<std::string>(yamlOut.error());
				T obj = yamlOut.unwrap();
				return json::toString(&obj);
			}

			template<Reflectable T>
			static Expected<std::string> fromBinaryVector(const std::vector<uint8_t>& vector) {
				Expected<T> binaryOut = binary::fromVector<T>(vector);
				if(binaryOut.isError()) return Expected<std::string>(binaryOut.error());
				T obj = binaryOut.unwrap();
				return json::toString(&obj);
			}

			template<Reflectable T>
			static Expected<std::string> fromBinaryStream(std::istream& stream) {
				Expected<T> binaryOut = binary::fromVector<T>(stream);
				if(binaryOut.isError()) return Expected<std::string>(binaryOut.error());
				T obj = binaryOut.unwrap();
				return json::toString(&obj);
			}
		};
		struct toYaml {//NOLINT namespace like name
			template<Reflectable T>
			static Expected<std::string> fromJsonString(std::string_view str) {
				Expected<T> jsonOut = json::fromString<T>(str);
				if(jsonOut.isError()) return Expected<std::string>(jsonOut.error());
				T obj = jsonOut.unwrap();
				return yaml::toString(&obj);
			}

			template<Reflectable T>
			static Expected<std::string> fromJsonStream(std::istream& stream) {
				Expected<T> jsonOut = json::fromString<T>(stream);
				if(jsonOut.isError()) return Expected<std::string>(jsonOut.error());
				T obj = jsonOut.unwrap();
				return yaml::toString(&obj);
			}

			template<Reflectable T>
			static Expected<std::string> fromBinaryVector(const std::vector<uint8_t>& vector) {
				Expected<T> binaryOut = binary::fromVector<T>(vector);
				if(binaryOut.isError()) return Expected<std::string>(binaryOut.error());
				T obj = binaryOut.unwrap();
				return yaml::toString(&obj);
			}

			template<Reflectable T>
			static Expected<std::string> fromBinaryStream(std::istream& stream) {
				Expected<T> binaryOut = binary::fromVector<T>(stream);
				if(binaryOut.isError()) return Expected<std::string>(binaryOut.error());
				T obj = binaryOut.unwrap();
				return yaml::toString(&obj);
			}
		};
		struct toBinary {//NOLINT namespace like name
			template<Reflectable T>
			static Expected<std::vector<uint8_t>> fromJsonString(std::string_view str) {
				Expected<T> jsonOut = json::fromString<T>(str);
				if(jsonOut.isError()) return Expected<std::vector<uint8_t>>(jsonOut.error());
				T obj = jsonOut.unwrap();
				return binary::toVector(&obj);
			}

			template<Reflectable T>
			static Expected<std::vector<uint8_t>> fromJsonStream(std::istream& stream) {
				Expected<T> jsonOut = json::fromString<T>(stream);
				if(jsonOut.isError()) return Expected<std::vector<uint8_t>>(jsonOut.error());
				T obj = jsonOut.unwrap();
				return binary::toVector(&obj);
			}

			template<Reflectable T>
			static Expected<std::vector<uint8_t>> fromYamlString(std::string_view str) {
				Expected<T> yamlOut = yaml::fromString<T>(str);
				if(yamlOut.isError()) return Expected<std::vector<uint8_t>>(yamlOut.error());
				T obj = yamlOut.unwrap();
				return binary::toVector(&obj);
			}

			template<Reflectable T>
			static Expected<std::vector<uint8_t>> fromYamlStream(std::istream& stream) {
				Expected<T> yamlOut = yaml::fromString<T>(stream);
				if(yamlOut.isError()) return Expected<std::vector<uint8_t>>(yamlOut.error());
				T obj = yamlOut.unwrap();
				return binary::toVector(&obj);
			}
		};
	};
}