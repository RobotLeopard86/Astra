#include "astra/yaml.hpp"

#include <ostream>
#include <cmath>
#include <stdexcept>
#include <string_view>

#include "astra/actions_table.hpp"
#include "astra/box.hpp"
#include "astra/type_info/list/list.hpp"
#include "astra/reflection.hpp"
#include "astra/stringify.hpp"
#include "astra/type_info.hpp"
#include "astra/type_info/integer/integer.hpp"
#include "astra/types/all_types.hpp"

namespace astra {
	void serializeYamlRecursive(YAML::Node _node, const TypeInfo& info, const std::string& field) {
		YAML::Node node = field.empty() ? _node : _node[field];
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool:
				node = info.asUnsafe<Bool>().get();
				break;
			case TypeInfo::Kind::Integer:
				if(auto intInfo = info.asUnsafe<Integer>(); intInfo.isSigned()) {
					node = ::astra::format("s{};{}", intInfo.size() * 8, intInfo.asSigned());
				} else {
					node = ::astra::format("u{};{}", intInfo.size() * 8, intInfo.asUnsigned());
				}
				break;
			case TypeInfo::Kind::Floating:
				if(auto floatInfo = info.asUnsafe<Floating>(); floatInfo.size() == sizeof(float)) {
					node = ::astra::format("{}f", (float)floatInfo.get());
				} else {
					node = std::to_string(floatInfo.get());
				}
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
					serializeYamlRecursive(_node, nestedInfo, field);
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
				if(!node.IsScalar()) throw std::runtime_error("Invalid integer format!");
				std::string fullNum = node.as<std::string>();
				std::string descriptor = fullNum.substr(0, fullNum.find_first_of(";")).substr(0, fullNum.find_first_of(";"));
				if(descriptor.compare(fullNum) == 0) throw std::runtime_error("Invalid integer format!");
				std::string numStr = fullNum.substr(descriptor.size() + 1);
				if(descriptor.size() < 2 || descriptor.size() > 3) throw std::runtime_error("Invalid integer format!");
				if(!(descriptor.ends_with("8") || descriptor.ends_with("16") || descriptor.ends_with("32") || descriptor.ends_with(64))) throw std::runtime_error("Invalid integer format!");
				if(descriptor[0] == 's') {
					info.asUnsafe<Integer>().setSigned(std::stoll(numStr));
				} else {
					info.asUnsafe<Integer>().setUnsigned(std::stoull(numStr));
				}
				break;
			}
			case TypeInfo::Kind::Floating: {
				if(!node.IsScalar()) throw std::runtime_error("Invalid floating-point format!");
				std::string fullNum = node.as<std::string>();
				if(char last = fullNum[fullNum.size() - 1]; last == 'f') {
					info.asUnsafe<Floating>().set(std::stof(fullNum.substr(0, fullNum.size() - 2)));
				} else if(last >= '0' && last <= '9') {
					info.asUnsafe<Floating>().set(std::stod(fullNum.substr(0, fullNum.size() - 2)));
				} else
					throw std::runtime_error("Invalid floating-point format!");
				break;
			}
			case TypeInfo::Kind::String:
				if(!node.IsScalar()) throw std::runtime_error("Invalid string format!");
				info.asUnsafe<String>().set(node.as<std::string>());
				break;
			case TypeInfo::Kind::Enum:
				if(!node.IsScalar()) throw std::runtime_error("Invalid enum format!");
				info.asUnsafe<Enum>().fromString(node.as<std::string>());
				break;
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
					ActionsTable::data()[tgt.type().number()].move(tgt.rawMut(), nested.var().rawMut());
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
					deserializeYamlRecursive(_node, nestedInfo, field);
				} catch(...) {
					p.init();
					TypeInfo okInfo = reflect(p.var());
					deserializeYamlRecursive(_node, okInfo, field);
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