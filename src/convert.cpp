#include "astra/serialized_convert.hpp"
#include "astra/format.hpp"
#include "bytestream.hpp"

#include "libjaguar/Document.hpp"
#include "libjaguar/Index.hpp"
#include "nlohmann/json.hpp"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"

namespace astra {
	std::string convert::yamlStringToJsonString(const std::string& str) {
		return yamlToJson(YAML::Load(str)).dump();
	}

	std::string convert::yamlStreamToJsonString(std::istream& stream) {
		return yamlToJson(YAML::Load(stream)).dump();
	}

	std::string convert::binaryVecToJsonString(const std::vector<uint8_t>& vector) {
		std::unique_ptr<ibytestream> ptr = std::make_unique<ibytestream>(const_cast<std::vector<uint8_t>&>(vector));
		libjaguar::Document doc(std::move(ptr));
		return binaryToJson(doc).dump();
	}

	std::string convert::binaryStreamToJsonString(std::istream& stream) {
		std::unique_ptr<std::istream> ptr(&stream);
		libjaguar::Document doc(std::move(ptr));
		doc.MaterializeAll();
		doc.ReleaseStream();
		return binaryToJson(doc).dump();
	}

	std::string convert::jsonStringToYamlString(const std::string& str) {
		YAML::Emitter em;
		em << jsonToYaml(nlohmann::json::parse(str));
		return em.c_str();
	}

	std::string convert::jsonStreamToYamlString(std::istream& stream) {
		nlohmann::json j;
		stream >> j;
		YAML::Emitter em;
		em << jsonToYaml(j);
		return em.c_str();
	}

	std::string convert::binaryVecToYamlString(const std::vector<uint8_t>& vector) {
		std::unique_ptr<ibytestream> ptr = std::make_unique<ibytestream>(const_cast<std::vector<uint8_t>&>(vector));
		libjaguar::Document doc(std::move(ptr));
		YAML::Emitter em;
		em << binaryToYaml(doc);
		return em.c_str();
	}

	std::string convert::binaryStreamToYamlString(std::istream& stream) {
		std::unique_ptr<std::istream> ptr(&stream);
		libjaguar::Document doc(std::move(ptr));
		doc.MaterializeAll();
		doc.ReleaseStream();
		YAML::Emitter em;
		em << binaryToYaml(doc);
		return em.c_str();
	}

	std::vector<uint8_t> convert::jsonStringToBinaryVec(const std::string& str) {
		libjaguar::Document doc = jsonToBinary(nlohmann::json::parse(str));
		std::vector<uint8_t> out;
		obytestream os(out);
		doc.ExportTo(os);
		return out;
	}

	std::vector<uint8_t> convert::jsonStreamToBinaryVec(std::istream& stream) {
		nlohmann::json j;
		stream >> j;
		libjaguar::Document doc = jsonToBinary(j);
		std::vector<uint8_t> out;
		obytestream os(out);
		doc.ExportTo(os);
		return out;
	}

	std::vector<uint8_t> convert::yamlStringToBinaryVec(const std::string& str) {
		libjaguar::Document doc = yamlToBinary(YAML::Load(str));
		std::vector<uint8_t> out;
		obytestream os(out);
		doc.ExportTo(os);
		return out;
	}

	std::vector<uint8_t> convert::yamlStreamToBinaryVec(std::istream& stream) {
		libjaguar::Document doc = yamlToBinary(YAML::Load(stream));
		std::vector<uint8_t> out;
		obytestream os(out);
		doc.ExportTo(os);
		return out;
	}

	nlohmann::json convert::yamlToJson(const YAML::Node& node) {
		//Force the emitter to output in JSON format
		YAML::Emitter em;
		em.SetStringFormat(YAML::EMITTER_MANIP::DoubleQuoted);
		em.SetMapFormat(YAML::EMITTER_MANIP::Flow);
		em.SetSeqFormat(YAML::EMITTER_MANIP::Flow);
		em << node;
		return nlohmann::json::parse(em.c_str());
	}

	YAML::Node convert::jsonToYaml(const nlohmann::json& json) {
		//YAML is superset of JSON so all JSON is valid
		return YAML::Load(json.dump());
	}

	YAML::Node convert::binaryToYaml(libjaguar::Document& doc) {
		//Go through JSON because it's fast and I don't want to rewrite logic
		return jsonToYaml(binaryToJson(doc));
	}

	libjaguar::Document convert::jsonToBinary(const nlohmann::json& json) {
		//Go through YAML because it's fast and I don't want to rewrite logic
		return yamlToBinary(jsonToYaml(json));
	}

	void binary2JsonRecursive(libjaguar::Document& doc, nlohmann::json& json, const std::string& path) {
		//Get scope information
		const libjaguar::ScopeEntry& scope = doc.QueryScopeInfo(path);

		//Handle values
		for(const libjaguar::ValueEntry& ve : scope.subvalues) {
			std::string effectivePath = (path.empty() ? ve.name : ::astra::format(scope.list ? "{}[{}]" : "{}.{}", path, ve.name));
			switch(ve.type) {
				case libjaguar::TypeTag::String:
					if(!scope.list) {
						json[ve.name] = doc.QueryValue<std::string>(effectivePath);
					} else {
						json[std::stoul(ve.name)] = doc.QueryValue<std::string>(effectivePath);
					}
					break;
				case libjaguar::TypeTag::ByteBuffer:
					if(!scope.list) {
						json[ve.name] = doc.QueryValue<std::vector<unsigned char>>(effectivePath);
					} else {
						json[std::stoul(ve.name)] = doc.QueryValue<std::vector<unsigned char>>(effectivePath);
					}
					break;
				case libjaguar::TypeTag::Boolean:
					if(!scope.list) {
						json[ve.name] = doc.QueryValue<bool>(effectivePath);
					} else {
						json[std::stoul(ve.name)] = doc.QueryValue<bool>(effectivePath);
					}
					break;
				case libjaguar::TypeTag::Float32:
					if(!scope.list) {
						json[ve.name] = std::to_string(doc.QueryValue<float>(effectivePath)) + "f";
					} else {
						json[std::stoul(ve.name)] = std::to_string(doc.QueryValue<float>(effectivePath)) + "f";
					}
					break;
				case libjaguar::TypeTag::Float64:
					if(!scope.list) {
						json[ve.name] = doc.QueryValue<double>(effectivePath);
					} else {
						json[std::stoul(ve.name)] = doc.QueryValue<double>(effectivePath);
					}
					break;
				case libjaguar::TypeTag::SInt8:
					if(!scope.list) {
						json[ve.name] = ::astra::format("s8;{}", doc.QueryValue<int8_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("s8;{}", doc.QueryValue<int8_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::SInt16:
					if(!scope.list) {
						json[ve.name] = ::astra::format("s16;{}", doc.QueryValue<int16_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("s16;{}", doc.QueryValue<int16_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::SInt32:
					if(!scope.list) {
						json[ve.name] = ::astra::format("s32;{}", doc.QueryValue<int32_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("s32;{}", doc.QueryValue<int32_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::SInt64:
					if(!scope.list) {
						json[ve.name] = ::astra::format("s64;{}", doc.QueryValue<int64_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("s64;{}", doc.QueryValue<int64_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::UInt8:
					if(!scope.list) {
						json[ve.name] = ::astra::format("u8;{}", doc.QueryValue<uint8_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("u8;{}", doc.QueryValue<uint8_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::UInt16:
					if(!scope.list) {
						json[ve.name] = ::astra::format("u16;{}", doc.QueryValue<uint16_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("u16;{}", doc.QueryValue<uint16_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::UInt32:
					if(!scope.list) {
						json[ve.name] = ::astra::format("u32;{}", doc.QueryValue<uint32_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("u32;{}", doc.QueryValue<uint32_t>(effectivePath));
					}
					break;
				case libjaguar::TypeTag::UInt64:
					if(!scope.list) {
						json[ve.name] = ::astra::format("u64;{}", doc.QueryValue<uint64_t>(effectivePath));
					} else {
						json[std::stoul(ve.name)] = ::astra::format("u64  ;{}", doc.QueryValue<uint64_t>(effectivePath));
					}
					break;
				default: break;
			}
		}

		//Handle scopes
		for(const libjaguar::ScopeEntry& se : scope.subscopes) {
			if(se.list) {
				json[se.name] = json.array();
				if(se.subscopes.size() < 1) continue;
				if(se.subscopes[0].subscopes.size() == 1 && se.subscopes[0].subvalues.size() == 0 && se.subscopes[0].subscopes[0].name.compare("payload") == 0) {
					for(std::size_t i = 0; i < se.subscopes.size(); ++i) {
						std::string subpath = ::astra::format("{}.{}[{}].payload", path.empty() ? "" : path + ".", se.name, i);
						if(subpath[0] == '.') subpath = subpath.substr(1);
						binary2JsonRecursive(doc, json[se.name][i], subpath);
					}
				} else {
					std::string subpath = (path.empty() ? "" : path + ".") + se.name;
					binary2JsonRecursive(doc, json[se.name], subpath);
				}
			} else {
				std::string effectivePath = (path.empty() ? se.name : ::astra::format(scope.list ? "{}[{}]" : "{}.{}", path, se.name));
				if(scope.list) {
					json[std::stoul(se.name)] = json.object();
					binary2JsonRecursive(doc, json[std::stoul(se.name)], effectivePath);
				} else {
					json[se.name] = json.object();
					binary2JsonRecursive(doc, json[se.name], effectivePath);
				}
			}
		}
	}

	nlohmann::json convert::binaryToJson(libjaguar::Document& doc) {
		nlohmann::json root;
		binary2JsonRecursive(doc, root, "");
		return root;
	}

	libjaguar::Document convert::yamlToBinary(const YAML::Node& node) {
	}
}