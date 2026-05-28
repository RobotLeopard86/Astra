#include "astra/json.hpp"

#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/type_info/enum/enum.hpp"
#include "astra/type_info/integer/integer.hpp"
#include "astra/type_info/list/list.hpp"
#include "astra/type_info/string/string.hpp"
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
					json = ::astra::format("{};s{}", intInfo.asSigned(), intInfo.size() * 8);
				} else {
					json = ::astra::format("{};u{}", intInfo.asSigned(), intInfo.size() * 8);
				}
				break;
			case TypeInfo::Kind::Floating:
				if(auto floatInfo = info.asUnsafe<Floating>(); floatInfo.size() == sizeof(float)) {
					json = ::astra::format("{}f", (float)floatInfo.get());
				} else {
					json = std::to_string(floatInfo.get());
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
			}
			case TypeInfo::Kind::List: {
				List list = info.asUnsafe<List>();
				TypeInfo nestedInfo = reflect(Var(nullptr, list.nestedType(), false));
				std::size_t i = 0;
				list.unsafeForEach([&](void* ptr) {
					nestedInfo.unsafeAssign(ptr);
					serializeJsonRecursive(json[i++], nestedInfo, "");
				});
			}
			case TypeInfo::Kind::Map: {
				Map map = info.asUnsafe<Map>();
				std::size_t i = 0;
				map.forEach([&](Var key, Var val) {
					nlohmann::json& entry = json[i++];
					serializeJsonRecursive(entry, reflect(key), "key");
					serializeJsonRecursive(entry, reflect(val), "val");
				});
			}
			case TypeInfo::Kind::Pointer: {
				const auto& p = info.asUnsafe<Pointer>();
				try {
					Var nested = p.getNested();
					serializeJsonRecursive(_json, reflect(nested), field);
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

	void deserializeJsonRecursive(const nlohmann::json& _json, TypeInfo& info) {
	}

	void json::deserialize(Var var, const nlohmann::json& json) {
		TypeInfo info = reflect(var);
		deserializeJsonRecursive(json, info);
	}
}