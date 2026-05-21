#pragma once

#include <cstddef>
#include <istream>

#include "astra/expected.hpp"
#include "astra/variable/var.hpp"
#include "astra/reflectable.hpp"

namespace astra {

	struct binary {//NOLINT namespace like name
		template<Reflectable T>
		static Expected<T> fromVector(const std::vector<uint8_t>& vector) {
			T obj;

			auto exp = deserialize(Var(&obj), vector);
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
		static Expected<std::vector<uint8_t>> toVector(const T* obj) {
			std::vector<uint8_t> result;
			serialize(&result, Var(obj));
			return result;
		}

		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			serialize(stream, Var(obj));
		}

		static void serialize(std::vector<uint8_t>* vector, Var var);
		static void serialize(std::ostream& stream, Var var);
		static Expected<None> deserialize(Var var, const std::vector<uint8_t>& vector);
		static Expected<None> deserialize(Var var, std::istream& stream);
	};

}