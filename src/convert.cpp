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

	libjaguar::Document convert::yamlToBinary(const YAML::Node& node) {
		//Go through JSON because it's fast and I don't want to rewrite logic
		return jsonToBinary(yamlToJson(node));
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
				if(se.subscopes.size() >= 1 && se.subscopes[0].subscopes.size() == 1 && se.subscopes[0].subvalues.size() == 0 && se.subscopes[0].subscopes[0].name.compare("payload") == 0) {
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

	void json2BinaryRecursive(libjaguar::Document& doc, const nlohmann::json& json, const std::string& path) {
		if(json.is_boolean()) {
			doc.SetOrCreateValue<bool>(path, json.get<bool>());
			return;
		}
		if(json.is_number_integer()) {
			doc.SetOrCreateValue<int64_t>(path, json.get<int64_t>());
			return;
		}
		if(json.is_number_unsigned()) {
			doc.SetOrCreateValue<uint64_t>(path, json.get<uint64_t>());
			return;
		}
		if(json.is_number_float()) {
			doc.SetOrCreateValue<double>(path, json.get<double>());
			return;
		}
		if(json.is_string()) {
			std::string str = json.get<std::string>();
			if(str.size() > 4 && (str[0] == 's' || str[0] == 'u')) {
				size_t semi = str.find(';');
				if(semi != std::string::npos) {
					std::string type = str.substr(0, semi);
					std::string value = str.substr(semi + 1);
					try {
						if(type == "s8") {
							doc.SetOrCreateValue<int8_t>(path, (int8_t)std::stoi(value));
							return;
						} else if(type == "s16") {
							doc.SetOrCreateValue<int16_t>(path, (int16_t)std::stoi(value));
							return;
						} else if(type == "s32") {
							doc.SetOrCreateValue<int32_t>(path, (int32_t)std::stol(value));
							return;
						} else if(type == "s64") {
							doc.SetOrCreateValue<int64_t>(path, (int64_t)std::stoll(value));
							return;
						} else if(type == "u8") {
							doc.SetOrCreateValue<uint8_t>(path, (uint8_t)std::stoul(value));
							return;
						} else if(type == "u16") {
							doc.SetOrCreateValue<uint16_t>(path, (uint16_t)std::stoul(value));
							return;
						} else if(type == "u32") {
							doc.SetOrCreateValue<uint32_t>(path, (uint32_t)std::stoul(value));
							return;
						} else if(type == "u64") {
							doc.SetOrCreateValue<uint64_t>(path, (uint64_t)std::stoull(value));
							return;
						}
					} catch(...) {}
				}
			}
			if(!str.empty() && str.back() == 'f') {
				try {
					doc.SetOrCreateValue<float>(path, std::stof(str.substr(0, str.size() - 1)));
					return;
				} catch(...) {}
			}
			doc.SetOrCreateValue(path, str);
			return;
		}
		if(json.is_object()) {
			if(!path.empty()) {
				doc.CreateValue<libjaguar::UnstructuredObjTag>(path);
			}
			for(auto it = json.begin(); it != json.end(); ++it) {
				std::string subpath = (path.empty() ? "" : path + ".") + it.key();
				json2BinaryRecursive(doc, it.value(), subpath);
			}
			return;
		}
		if(json.is_array()) {
			if(json.empty()) {
				doc.CreateValue<std::vector<libjaguar::UnstructuredObjTag>>(path);
				return;
			}
			const nlohmann::json& first = json[0];
			if(first.is_boolean()) {
				doc.CreateValue<std::vector<bool>>(path);
			} else if(first.is_number_integer()) {
				doc.CreateValue<std::vector<int64_t>>(path);
			} else if(first.is_number_unsigned()) {
				doc.CreateValue<std::vector<uint64_t>>(path);
			} else if(first.is_number_float()) {
				doc.CreateValue<std::vector<double>>(path);
			} else if(first.is_string()) {
				std::string str = first.get<std::string>();
				if(str.starts_with("s8;")) {
					doc.CreateValue<std::vector<int8_t>>(path);
				} else if(str.starts_with("s16;")) {
					doc.CreateValue<std::vector<int16_t>>(path);
				} else if(str.starts_with("s32;")) {
					doc.CreateValue<std::vector<int32_t>>(path);
				} else if(str.starts_with("s64;")) {
					doc.CreateValue<std::vector<int64_t>>(path);
				} else if(str.starts_with("u8;")) {
					doc.CreateValue<std::vector<uint8_t>>(path, true);
				} else if(str.starts_with("u16;")) {
					doc.CreateValue<std::vector<uint16_t>>(path);
				} else if(str.starts_with("u32;")) {
					doc.CreateValue<std::vector<uint32_t>>(path);
				} else if(str.starts_with("u64;")) {
					doc.CreateValue<std::vector<uint64_t>>(path);
				} else if(!str.empty() && str.back() == 'f') {
					doc.CreateValue<std::vector<float>>(path);
				} else {
					doc.CreateValue<std::vector<std::string>>(path);
				}
			} else {
				doc.CreateValue<std::vector<libjaguar::UnstructuredObjTag>>(path);
			}
			for(std::size_t i = 0; i < json.size(); ++i) {
				std::string subpath = path + "[" + std::to_string(i) + "]";
				if(json[i].is_array()) {
					doc.CreateValue<libjaguar::UnstructuredObjTag>(subpath);
					json2BinaryRecursive(doc, json[i], subpath + ".payload");
				} else {
					json2BinaryRecursive(doc, json[i], subpath);
				}
			}
			return;
		}
	}

	libjaguar::Document convert::jsonToBinary(const nlohmann::json& json) {
		libjaguar::Document doc;
		json2BinaryRecursive(doc, json, "");
		return doc;
	}
}