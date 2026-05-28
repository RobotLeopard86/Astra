#pragma once

#include <istream>

#include "dll.hpp"
#include "var.hpp"
#include "reflectable.hpp"

#include "nlohmann/json.hpp"

namespace astra {
	struct ASTRA_API json {
		template<Reflectable T>
		static T fromJson(const nlohmann::json& json) {
			T obj;
			deserialize(Var(&obj), json);
			return obj;
		}

		template<Reflectable T>
		static T fromString(const std::string& str) {
			return fromJson<T>(nlohmann::json::parse(str));
		}

		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			nlohmann::json j;
			stream >> j;
			return fromJson<T>(j);
		}

		template<Reflectable T>
		static nlohmann::json toJSON(const T* obj) {
			nlohmann::json j;
			serialize(j, Var(obj));
			return j;
		}

		template<Reflectable T>
		static std::string toString(const T* obj) {
			return toJSON<T>(obj).dump();
		}

		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			stream << toJSON<T>(obj);
		}

	  private:
		static void serialize(nlohmann::json& json, Var var);
		static void deserialize(Var var, const nlohmann::json& json);
	};
}