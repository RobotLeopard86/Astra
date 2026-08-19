#include "astra/json.hpp"

#include "astra/type_table.hpp"
#include "astra/box.hpp"
#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/types/enum/enum.hpp"
#include "astra/types/integer/integer.hpp"
#include "astra/types/list/list.hpp"
#include "astra/types/string/string.hpp"

#include "nlohmann/json.hpp"

namespace astra {
	void serializeJsonRecursive(nlohmann::json& _json, const TypeInfo& info, const std::string& field) {
		nlohmann::json& json = field.empty() ? _json : _json[field];
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool:
				json = info.asUnsafe<Bool>().get();
				break;
			case TypeInfo::Kind::Integer:
				if(auto intInfo = info.asUnsafe<Integer>(); intInfo.isSigned()) {
					json = intInfo.asSigned();
				} else {
					json = intInfo.asUnsigned();
				}
				break;
			case TypeInfo::Kind::Float:
				if(auto floatInfo = info.asUnsafe<Float>(); floatInfo.size() == sizeof(float)) {
					json = static_cast<float>(floatInfo.get());
				} else {
					json = floatInfo.get();
				}
				break;
			case TypeInfo::Kind::String:
				json = info.asUnsafe<String>().get();
				break;
			case TypeInfo::Kind::Enum:
				json = info.asUnsafe<Enum>().toString();
				break;
			case TypeInfo::Kind::Object:
				for(const auto& [name, contents] : info.asUnsafe<Object>().getFields()) {
					serializeJsonRecursive(json, reflect(contents.var()), std::string(name));
				}
				break;
			case TypeInfo::Kind::Array: {
				Array arr = info.asUnsafe<Array>();
				TypeInfo nestedInfo = reflect(Var(nullptr, arr.nestedType(), false));
				std::size_t i = 0;
				arr.unsafeForEach([&](void* ptr) {
					nestedInfo.unsafeAssign(ptr);
					serializeJsonRecursive(json[i++], nestedInfo, "");
				});
				break;
			}
			case TypeInfo::Kind::List: {
				List list = info.asUnsafe<List>();
				TypeInfo nestedInfo = reflect(Var(nullptr, list.nestedType(), false));
				std::size_t i = 0;
				list.unsafeForEach([&](void* ptr) {
					nestedInfo.unsafeAssign(ptr);
					serializeJsonRecursive(json[i++], nestedInfo, "");
				});
				break;
			}
			case TypeInfo::Kind::Map: {
				Map map = info.asUnsafe<Map>();
				std::size_t i = 0;
				map.forEach([&](Var key, Var val) {
					nlohmann::json& entry = json[i++];
					serializeJsonRecursive(entry, reflect(key), "key");
					serializeJsonRecursive(entry, reflect(val), "val");
				});
				break;
			}
			case TypeInfo::Kind::Pointer: {
				const auto& p = info.asUnsafe<Pointer>();
				try {
					Var nested = p.getNested();
					serializeJsonRecursive(json, reflect(nested), field);
				} catch(...) {
					//Can't serialize nullptr
				}
				break;
			}
		}
	}

	void json::serialize(nlohmann::json& json, Var var) {
		serializeJsonRecursive(json, reflect(var), "");
	}

	void deserializeJsonRecursive(const nlohmann::json& _json, TypeInfo& info, const std::string& field) {
		const nlohmann::json& json = field.empty() ? _json : _json[field];
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool:
				if(!json.is_boolean()) throw std::runtime_error("Invalid boolean format!");
				info.asUnsafe<Bool>().set(json.get<bool>());
				break;
			case TypeInfo::Kind::Integer: {
				Integer intInfo = info.asUnsafe<Integer>();
				if(intInfo.isSigned()) {
					if(!json.is_number_integer()) throw std::runtime_error("Invalid integer format!");
					intInfo.setSigned(json.get<int64_t>());
				} else {
					if(!json.is_number_unsigned()) throw std::runtime_error("Invalid integer format!");
					intInfo.setUnsigned(json.get<uint64_t>());
				}
				break;
			}
			case TypeInfo::Kind::Float: {
				if(!json.is_number_float()) throw std::runtime_error("Invalid floating-point format!");
				Float floatInfo = info.asUnsafe<Float>();
				if(floatInfo.size() == sizeof(float)) {
					floatInfo.set(json.get<float>());
				} else {
					floatInfo.set(json.get<double>());
				}
				break;
			}
			case TypeInfo::Kind::String: {
				if(!json.is_string()) throw std::runtime_error("Invalid string format!");
				info.asUnsafe<String>().set(json.get<std::string>());
				break;
			}
			case TypeInfo::Kind::Enum: {
				if(!json.is_string()) throw std::runtime_error("Invalid enum format!");
				info.asUnsafe<Enum>().fromString(json.get<std::string>());
				break;
			}
			case TypeInfo::Kind::Object: {
				if(!json.is_object()) throw std::runtime_error("Invalid object format!");
				Object obj = info.asUnsafe<Object>();
				for(const auto& kv : json.items()) {
					FieldInfo objField = obj.getField(kv.key());
					TypeInfo fieldInfo = reflect(objField.var());
					deserializeJsonRecursive(kv.value(), fieldInfo, "");
				}
				break;
			}
			case TypeInfo::Kind::Array: {
				if(!json.is_array()) throw std::runtime_error("Invalid array format!");
				Array arr = info.asUnsafe<Array>();
				std::size_t i = 0;
				for(const nlohmann::json& subjson : json) {
					Box nested(arr.nestedType());
					TypeInfo nestedInfo = reflect(nested.var());
					deserializeJsonRecursive(subjson, nestedInfo, "");
					if(i >= arr.size()) throw std::runtime_error("Too many items in array!");
					Var tgt = arr.at(i);
					if(tgt.isConst()) throw std::runtime_error("Cannot deserialize into const array!");
					TypeTable::actions()[tgt.typeId().number()].move(tgt.rawMut(), nested.var().rawMut());
				}
				break;
			}
			case TypeInfo::Kind::List: {
				if(!json.is_array()) throw std::runtime_error("Invalid list format!");
				List list = info.asUnsafe<List>();
				list.clear();
				for(const nlohmann::json& subjson : json) {
					Box nested(list.nestedType());
					TypeInfo nestedInfo = reflect(nested.var());
					deserializeJsonRecursive(subjson, nestedInfo, "");
					list.push(nested.var());
				}
				break;
			}
			case TypeInfo::Kind::Map: {
				if(!json.is_array()) throw std::runtime_error("Invalid map format!");
				Map map = info.asUnsafe<Map>();
				map.clear();
				for(const nlohmann::json& subjson : json) {
					if(!subjson.is_object()) throw std::runtime_error("Invalid map format!");
					Box key(map.keyType()), val(map.valType());
					TypeInfo keyInfo = reflect(key.var()), valInfo = reflect(val.var());
					deserializeJsonRecursive(subjson, keyInfo, "key");
					deserializeJsonRecursive(subjson, valInfo, "val");
					map.insert(key.var(), val.var());
				}
				break;
			}
			case TypeInfo::Kind::Pointer: {
				Pointer& p = info.asUnsafe<Pointer>();
				try {
					TypeInfo nestedInfo = reflect(p.getNested());
					deserializeJsonRecursive(json, nestedInfo, field);
				} catch(...) {
					p.init();
					TypeInfo okInfo = reflect(p.var());
					deserializeJsonRecursive(json, okInfo, field);
				}
				break;
			};
		}
	}

	void json::deserialize(Var var, const nlohmann::json& json) {
		TypeInfo info = reflect(var);
		deserializeJsonRecursive(json, info, "");
	}
}