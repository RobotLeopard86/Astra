#pragma once

#include "binary.hpp"
#include "json.hpp"
#include "yaml.hpp"

namespace astra {
	struct ASTRA_API converters {
		struct ASTRA_API toJson {
			template<Reflectable T>
			static std::string fromYamlString(const std::string& str) {
				T yamlOut;
				try {
					yamlOut = yaml::fromString<T>(str);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = yamlOut;
				return json::toString(&obj);
			}

			template<Reflectable T>
			static std::string fromYamlStream(std::istream& stream) {
				T yamlOut;
				try {
					yamlOut = yaml::fromString<T>(stream);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = yamlOut;
				return json::toString(&obj);
			}

			template<Reflectable T>
			static std::string fromBinaryVector(const std::vector<uint8_t>& vector) {
				T binaryOut;
				try {
					binaryOut = binary::fromVector<T>(vector);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = binaryOut;
				return json::toString(&obj);
			}

			template<Reflectable T>
			static std::string fromBinaryStream(std::istream& stream) {
				T binaryOut;
				try {
					binaryOut = binary::fromVector<T>(stream);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = binaryOut;
				return json::toString(&obj);
			}
		};
		struct ASTRA_API toYaml {
			template<Reflectable T>
			static std::string fromJsonString(const std::string& str) {
				T jsonOut;
				try {
					jsonOut = json::fromString<T>(str);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = jsonOut;
				return yaml::toString(&obj);
			}

			template<Reflectable T>
			static std::string fromJsonStream(std::istream& stream) {
				T jsonOut;
				try {
					jsonOut = json::fromString<T>(stream);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = jsonOut;
				return yaml::toString(&obj);
			}

			template<Reflectable T>
			static std::string fromBinaryVector(const std::vector<uint8_t>& vector) {
				T binaryOut;
				try {
					binaryOut = binary::fromVector<T>(vector);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = binaryOut;
				return yaml::toString(&obj);
			}

			template<Reflectable T>
			static std::string fromBinaryStream(std::istream& stream) {
				T binaryOut;
				try {
					binaryOut = binary::fromVector<T>(stream);
				} catch(const std::exception& e) {
					return std::string(e.what());
				}
				T obj = binaryOut;
				return yaml::toString(&obj);
			}
		};
		struct ASTRA_API toBinary {
			template<Reflectable T>
			static std::vector<uint8_t> fromJsonString(const std::string& str) {
				T jsonOut;
				try {
					jsonOut = json::fromString<T>(str);
				} catch(const std::exception& e) {
					return std::vector<uint8_t>(jsonOut.error());
				}
				T obj = jsonOut;
				return binary::toVector(&obj);
			}

			template<Reflectable T>
			static std::vector<uint8_t> fromJsonStream(std::istream& stream) {
				T jsonOut;
				try {
					jsonOut = json::fromString<T>(stream);
				} catch(const std::exception& e) {
					return std::vector<uint8_t>(jsonOut.error());
				}
				T obj = jsonOut;
				return binary::toVector(&obj);
			}

			template<Reflectable T>
			static std::vector<uint8_t> fromYamlString(const std::string& str) {
				T yamlOut;
				try {
					yamlOut = yaml::fromString<T>(str);
				} catch(const std::exception& e) {
					return std::vector<uint8_t>(yamlOut.error());
				}
				T obj = yamlOut;
				return binary::toVector(&obj);
			}

			template<Reflectable T>
			static std::vector<uint8_t> fromYamlStream(std::istream& stream) {
				T yamlOut;
				try {
					yamlOut = yaml::fromString<T>(stream);
				} catch(const std::exception& e) {
					return std::vector<uint8_t>(yamlOut.error());
				}
				T obj = yamlOut;
				return binary::toVector(&obj);
			}
		};
	};
}