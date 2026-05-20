#pragma once

#include <istream>
#include <string>

#include "astra/expected.hpp"
#include "astra/variable/var.hpp"
#include "astra/reflectable.hpp"

namespace astra {

	struct yaml {//NOLINT namespace like name
		template<Reflectable T>
		static Expected<T> fromString(std::string_view str) {
			T obj;

			auto exp = deserialize(Var(&obj), str);
			if(exp.isError()) {
				return exp.error();
			}
			return obj;
		}

		template<Reflectable T>
		static Expected<T> fromStream(std::istream& stream) {
			T obj;

			auto exp = deserialize(Var(&obj), stream);
			if(exp.isError()) {
				return exp.error();
			}
			return obj;
		}

		template<Reflectable T>
		static Expected<std::string> toString(const T* obj) {
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
		static Expected<None> deserialize(Var var, std::string_view str);
		static Expected<None> deserialize(Var var, std::istream& stream);
	};

}