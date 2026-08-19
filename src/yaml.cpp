#include "astra/yaml.hpp"
#include "astra/type_table.hpp"
#include "astra/box.hpp"
#include "astra/types/list/list.hpp"
#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/types/integer/integer.hpp"


#include <stdexcept>

#include "yaml-cpp/exceptions.h"

namespace astra {
	void serializeYamlRecursive(YAML::Node _node, const TypeInfo& info, const std::string& field) {
		YAML::Node node = field.empty() ? _node : _node[field];
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool:
				node = info.asUnsafe<Bool>().get();
				break;
			case TypeInfo::Kind::Integer:
				if(auto intInfo = info.asUnsafe<Integer>(); intInfo.isSigned()) {
					node = intInfo.asSigned();
				} else {
					node = intInfo.asUnsigned();
				}
				break;
			case TypeInfo::Kind::Float:
				if(auto floatInfo = info.asUnsafe<Float>(); floatInfo.size() == sizeof(float)) {
					node = static_cast<float>(floatInfo.get());
				} else {
					node = floatInfo.get();
				}
				break;
			case TypeInfo::Kind::String:
				node = info.asUnsafe<String>().get();
				break;
			case TypeInfo::Kind::Enum:
				node = info.asUnsafe<Enum>().toString();
				break;
			case TypeInfo::Kind::Object:
				for(const auto& [name, contents] : info.asUnsafe<Object>().getFields()) {
					serializeYamlRecursive(node, reflect(contents.var()), std::string(name));
				}
				break;
			case TypeInfo::Kind::Array: {
				Array arr = info.asUnsafe<Array>();
				TypeInfo nestedInfo = reflect(Var(nullptr, arr.nestedType(), false));
				std::size_t i = 0;
				arr.unsafeForEach([&](void* ptr) {
					nestedInfo.unsafeAssign(ptr);
					serializeYamlRecursive(node[i++], nestedInfo, "");
				});
				break;
			}
			case TypeInfo::Kind::List: {
				List list = info.asUnsafe<List>();
				TypeInfo nestedInfo = reflect(Var(nullptr, list.nestedType(), false));
				std::size_t i = 0;
				list.unsafeForEach([&](void* ptr) {
					nestedInfo.unsafeAssign(ptr);
					serializeYamlRecursive(node[i++], nestedInfo, "");
				});
				break;
			}
			case TypeInfo::Kind::Map: {
				Map map = info.asUnsafe<Map>();
				std::size_t i = 0;
				map.forEach([&](Var key, Var val) {
					YAML::Node subnode = node[i++];
					serializeYamlRecursive(subnode, reflect(key), "key");
					serializeYamlRecursive(subnode, reflect(val), "val");
				});
				break;
			}
			case TypeInfo::Kind::Pointer: {
				const auto& p = info.asUnsafe<Pointer>();
				try {
					TypeInfo nestedInfo = reflect(p.getNested());
					serializeYamlRecursive(node, nestedInfo, field);
				} catch(...) {
					//Can't serialize nullptr
				}
				break;
			}
		}
	}

	void yaml::serialize(YAML::Node& node, Var var) {
		serializeYamlRecursive(node, reflect(var), "");
	}

	void deserializeYamlRecursive(YAML::Node _node, TypeInfo& info, const std::string& field) {
		YAML::Node node = field.empty() ? _node : _node[field];
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool:
				if(!node.IsScalar()) throw std::runtime_error("Invalid boolean format!");
				info.asUnsafe<Bool>().set(node.as<bool>());
				break;
			case TypeInfo::Kind::Integer: {
				Integer intInfo = info.asUnsafe<Integer>();
				if(intInfo.isSigned()) {
					if(!node.IsScalar()) throw std::runtime_error("Invalid integer format!");
					intInfo.setSigned(node.as<int64_t>());
				} else {
					if(!node.IsScalar()) throw std::runtime_error("Invalid integer format!");
					if(node.Scalar()[0] == '-') throw std::runtime_error("Invalid integer format!");
					intInfo.setUnsigned(node.as<uint64_t>());
				}
				break;
			}
			case TypeInfo::Kind::Float: {
				if(!node.IsScalar()) throw std::runtime_error("Invalid floating-point format!");
				Float floatInfo = info.asUnsafe<Float>();
				try {
					if(floatInfo.size() == sizeof(float)) {
						floatInfo.set(node.as<float>());
					} else {
						floatInfo.set(node.as<double>());
					}
				} catch(const YAML::Exception&) {
					throw std::runtime_error("Invalid floating-point format!");
				}
				break;
			}
			case TypeInfo::Kind::String: {
				if(!node.IsScalar()) throw std::runtime_error("Invalid string format!");
				info.asUnsafe<String>().set(node.as<std::string>());
				break;
			}
			case TypeInfo::Kind::Enum: {
				if(!node.IsScalar()) throw std::runtime_error("Invalid enum format!");
				info.asUnsafe<Enum>().fromString(node.as<std::string>());
				break;
			}
			case TypeInfo::Kind::Object: {
				if(!node.IsMap()) throw std::runtime_error("Invalid object format!");
				Object obj = info.asUnsafe<Object>();
				for(auto it = node.begin(); it != node.end(); ++it) {
					FieldInfo objField = obj.getField(it->first.as<std::string>());
					TypeInfo fieldInfo = reflect(objField.var());
					deserializeYamlRecursive(it->second, fieldInfo, "");
				}
				break;
			}
			case TypeInfo::Kind::Array: {
				if(!node.IsSequence()) throw std::runtime_error("Invalid array format!");
				Array arr = info.asUnsafe<Array>();
				std::size_t i = 0;
				for(const YAML::Node& subnode : node) {
					Box nested(arr.nestedType());
					TypeInfo nestedInfo = reflect(nested.var());
					deserializeYamlRecursive(subnode, nestedInfo, "");
					if(i >= arr.size()) throw std::runtime_error("Too many items in array!");
					Var tgt = arr.at(i);
					if(tgt.isConst()) throw std::runtime_error("Cannot deserialize into const array!");
					TypeTable::actions()[tgt.typeId().number()].move(tgt.rawMut(), nested.var().rawMut());
				}
				break;
			}
			case TypeInfo::Kind::List: {
				if(!node.IsSequence()) throw std::runtime_error("Invalid list format!");
				List list = info.asUnsafe<List>();
				list.clear();
				for(const YAML::Node& subnode : node) {
					Box nested(list.nestedType());
					TypeInfo nestedInfo = reflect(nested.var());
					deserializeYamlRecursive(subnode, nestedInfo, "");
					list.push(nested.var());
				}
				break;
			}
			case TypeInfo::Kind::Map: {
				if(!node.IsSequence()) throw std::runtime_error("Invalid map format!");
				Map map = info.asUnsafe<Map>();
				map.clear();
				for(const YAML::Node& subnode : node) {
					if(!subnode.IsMap()) throw std::runtime_error("Invalid map format!");
					Box key(map.keyType()), val(map.valType());
					TypeInfo keyInfo = reflect(key.var()), valInfo = reflect(val.var());
					deserializeYamlRecursive(subnode, keyInfo, "key");
					deserializeYamlRecursive(subnode, valInfo, "val");
					map.insert(key.var(), val.var());
				}
				break;
			}
			case TypeInfo::Kind::Pointer: {
				Pointer& p = info.asUnsafe<Pointer>();
				try {
					TypeInfo nestedInfo = reflect(p.getNested());
					deserializeYamlRecursive(node, nestedInfo, field);
				} catch(...) {
					p.init();
					TypeInfo okInfo = reflect(p.var());
					deserializeYamlRecursive(node, okInfo, field);
				}
				break;
			};
		}
	}

	void yaml::deserialize(Var var, const YAML::Node& node) {
		TypeInfo info = reflect(var);
		deserializeYamlRecursive(node, info, "");
	}
}