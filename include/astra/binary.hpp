#pragma once

#include <istream>

#include "var.hpp"
#include "reflectable.hpp"
#include "dll.hpp"

#include "libjaguar/Document.hpp"

namespace astra {
	struct ASTRA_API binary {
		template<Reflectable T>
		static T fromDocument(libjaguar::Document& doc) {
			T obj;
			deserialize(Var(&obj), doc);
			return obj;
		}

		template<Reflectable T>
		static T fromVector(const std::vector<uint8_t>& vector) {
			T obj;
			deserialize(Var(&obj), vector);
			return obj;
		}

		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			T obj;
			deserialize(Var(&obj), stream);
			return obj;
		}

		template<Reflectable T>
		static libjaguar::Document toDocument(const T* obj) {
			libjaguar::Document doc;
			serialize(doc, Var(obj));
			return doc;
		}

		template<Reflectable T>
		static std::vector<uint8_t> toVector(const T* obj) {
			std::vector<uint8_t> result;
			serialize(result, Var(obj));
			return result;
		}

		template<Reflectable T>
		static void toStream(std::ostream& stream, const T* obj) {
			toDocument<T>(obj).ExportTo(stream);
		}

	  private:
		static void serialize(std::vector<uint8_t>& vector, Var var);
		static void serialize(std::ostream& stream, Var var);
		static void serialize(libjaguar::Document& doc, Var var);
		static void deserialize(Var var, const std::vector<uint8_t>& vector);
		static void deserialize(Var var, std::istream& stream);
		static void deserialize(Var var, libjaguar::Document& doc);
	};
}