#include "astra/yaml.hpp"

#include <ostream>
#include <cmath>
#include <string_view>

#include "parser_yaml.hpp"

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
					node = ::astra::format("{};s{}", intInfo.asSigned(), intInfo.size() * 8);
				} else {
					node = ::astra::format("{};u{}", intInfo.asSigned(), intInfo.size() * 8);
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
			}
			case TypeInfo::Kind::Sequence: {
				Sequence seq = info.asUnsafe<Sequence>();
				TypeInfo nestedInfo = reflect(Var(nullptr, seq.nestedType(), false));
				std::size_t i = 0;
				seq.unsafeForEach([&](void* ptr) {
					nestedInfo.unsafeAssign(ptr);
					serializeYamlRecursive(node[i++], nestedInfo, "");
				});
			}
			case TypeInfo::Kind::Map: {
				Map map = info.asUnsafe<Map>();
				std::size_t i = 0;
				map.forEach([&](Var key, Var val) {
					YAML::Node subnode = node[i++];
					serializeYamlRecursive(subnode, reflect(key), "key");
					serializeYamlRecursive(subnode, reflect(val), "val");
				});
			}
			case TypeInfo::Kind::Pointer: {
				const auto& p = info.asUnsafe<Pointer>();
				try {
					Var nested = p.getNested();
					serializeYamlRecursive(node, reflect(nested), field);
				} catch(...) {
					//Can't serialize nullptr
				}
				break;
			}
		}
	}

	void yaml::serialize(YAML::Node& node, Var var) {
		auto info = reflect(var);
		serializeYamlRecursive(node, info, "");
	}

	void yaml::deserialize(Var var, const YAML::Node& node) {
		//TODO: business logic
	}
}