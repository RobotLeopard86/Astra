#include "astra/json.hpp"

#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/type_info/enum/enum.hpp"
#include "astra/type_info/integer/integer.hpp"
#include "astra/type_info/sequence/sequence.hpp"
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
					json = intInfo.asSigned();
				} else {
					json = intInfo.asUnsigned();
				}
				break;
			case TypeInfo::Kind::Floating:
				json = info.asUnsafe<Floating>().get();
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
			case TypeInfo::Kind::Sequence: {
				Sequence seq = info.asUnsafe<Sequence>();
				TypeInfo nestedInfo = reflect(Var(nullptr, seq.nestedType(), false));
				std::size_t i = 0;
				seq.unsafeForEach([&](void* ptr) {
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
					serializeJsonRecursive(json, reflect(nested), field);
				} catch(...) {
					//Can't serialize nullptr
				}
				break;
			}
		}
	}

	void json::serialize(nlohmann::json& json, Var var) {
		auto info = reflect(var);
		serializeJsonRecursive(json, info, "");
	}

	void json::deserialize(Var var, const nlohmann::json& json) {
		//TODO: business logic
	}
}