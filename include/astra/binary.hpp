#pragma once

#include <istream>

#include "var.hpp"
#include "reflectable.hpp"
#include "dll.hpp"

#include "libjaguar/Document.hpp"

namespace astra {
	struct ASTRA_API binary {
		/**
		 * @brief Deserialize a T object from the Jaguar document
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param doc The Jaguar document to deserialize from
		 *
		 * @return A T object storing the data from the document
		 */
		template<Reflectable T>
		static T fromDocument(libjaguar::Document& doc) {
			T obj;
			deserialize(Var(&obj), doc);
			return obj;
		}

		/**
		 * @brief Deserialize a T object from a vector of Jaguar data
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param vector A vector storing Jaguar data to deserialize from
		 *
		 * @return A T object storing the data from the vector
		 */
		template<Reflectable T>
		static T fromVector(const std::vector<uint8_t>& vector) {
			T obj;
			deserialize(Var(&obj), vector);
			return obj;
		}

		/**
		 * @brief Deserialize a T object from a stream of Jaguar data
		 *
		 * @tparam T The reflectable type to deserialize to
		 *
		 * @param stream A stream storing Jaguar data to deserialize from
		 *
		 * @return A T object storing the data from the stream
		 */
		template<Reflectable T>
		static T fromStream(std::istream& stream) {
			T obj;
			deserialize(Var(&obj), stream);
			return obj;
		}

		/**
		 * @brief Serialize a T object to a Jaguar document
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A document containing the serialized data
		 */
		template<Reflectable T>
		static libjaguar::Document toDocument(const T* obj) {
			libjaguar::Document doc;
			serialize(doc, Var(obj));
			return doc;
		}

		/**
		 * @brief Serialize a T object to a vector of Jaguar data
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A vector containing the serialized data encoded in Jaguar format
		 */
		template<Reflectable T>
		static std::vector<uint8_t> toVector(const T* obj) {
			std::vector<uint8_t> result;
			serialize(result, Var(obj));
			return result;
		}

		/**
		 * @brief Serialize a T object to a stream as Jaguar data
		 *
		 * @tparam T The reflectable type to serialize from
		 *
		 * @param stream The stream to write the serialized Jaguar data to
		 * @param obj The object to serialize
		 */
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