#pragma once

#include <istream>
#include <string>

#include "dll.hpp"
#include "astra/var.hpp"
#include "astra/reflectable.hpp"

namespace astra {

	struct ASTRA_API yaml {
		template<Reflectable T>
		static T fromString(std::string_view str) {

			T obj;
			deserialize(Var(&obj), str);
			return obj;
		}

		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			T obj;
			deserialize(Var(&obj), stream);
			return obj;
		}

		template<Reflectable T>
		static std::string toString(const T* obj) {
			std::string result;
			serialize(&result, Var(obj));
			return result;
		}

		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			serialize(stream, Var(obj));
		}

	  private:
		static void serialize(std::string* str, Var var);
		static void serialize(std::ostream& stream, Var var);
		static void deserialize(Var var, std::string_view str);
		static void deserialize(Var var, std::istream& stream);
	};
}