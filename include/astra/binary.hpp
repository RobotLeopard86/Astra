#pragma once

#include <istream>

#include "var.hpp"
#include "reflectable.hpp"
#include "dll.hpp"

#include "libjaguar/Document.hpp"

namespace astra {
	/**
	 * @brief Helper for serializing and deserializing objects to/from binary data (encoded as Jaguar data)
	 */
	struct ASTRA_API binary {
		/**
		 * @brief Deserialize a T object from the Jaguar document
		 *
		 * @tparam T The serializable type to deserialize to
		 *
		 * @param doc The Jaguar document to deserialize from
		 *
		 * @return A T object storing the data from the document
		 */
		template<Serializable T>
		static T fromDocument(libjaguar::Document& doc) {
			using S = SerializedSubstitute<T>;
			S obj;
			deserialize(Var(&obj), doc);
			T out;
			obj.deserialize(&out);
			return out;
		}

		///@cond
		template<Serializable T>
			requires std::is_enum_v<T>
		static T fromDocument(libjaguar::Document& doc) {
			T obj;
			deserialize(Var(&obj), doc);
			return obj;
		}
		///@endcond

		/**
		 * @brief Deserialize a T object from a vector of Jaguar data
		 *
		 * @tparam T The serializable type to deserialize to
		 *
		 * @param vector A vector storing Jaguar data to deserialize from
		 *
		 * @return A T object storing the data from the vector
		 */
		template<Serializable T>
		static T fromVector(const std::vector<uint8_t>& vector) {
			using S = SerializedSubstitute<T>;
			S obj;
			deserialize(Var(&obj), vector);
			T out;
			obj.deserialize(&out);
			return out;
		}

		///@cond
		template<Serializable T>
			requires std::is_enum_v<T>
		static T fromVector(const std::vector<uint8_t>& vector) {
			T obj;
			deserialize(Var(&obj), vector);
			return obj;
		}
		///@endcond

		/**
		 * @brief Deserialize a T object from a stream of Jaguar data
		 *
		 * @tparam T The serializable type to deserialize to
		 *
		 * @param stream A stream storing Jaguar data to deserialize from
		 *
		 * @return A T object storing the data from the stream
		 */
		template<Serializable T>
		static T fromStream(std::istream& stream) {
			using S = SerializedSubstitute<T>;
			S obj;
			deserialize(Var(&obj), stream);
			T out;
			obj.deserialize(&out);
			return out;
		}

		///@cond
		template<Serializable T>
			requires std::is_enum_v<T>
		static T fromStream(std::istream& stream) {
			T obj;
			deserialize(Var(&obj), stream);
			return obj;
		}
		///@endcond

		/**
		 * @brief Deserialize a Jaguar document into a Var
		 *
		 * @param doc The Jaguar document to deserialize from
		 * @param var The Var to write into
		 */
		static void fromDocumentIntoVar(libjaguar::Document& doc, Var var);

		/**
		 * @brief Deserialize a vector of Jaguar data into a Var
		 *
		 * @param vector A vector storing Jaguar data to deserialize from
		 * @param var The Var to write into
		 */
		static void fromVectorIntoVar(const std::vector<uint8_t>& vector, Var var);

		/**
		 * @brief Deserialize a stream of Jaguar data into a Var
		 *
		 * @param stream A stream storing Jaguar data to deserialize from
		 * @param var The Var to write into
		 */
		static void fromStreamIntoVar(std::istream& stream, Var var);

		/**
		 * @brief Serialize a T object to a Jaguar document
		 *
		 * @tparam T The serializable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A document containing the serialized data
		 */
		template<Serializable T>
		static libjaguar::Document toDocument(const T* obj) {
			using S = SerializedSubstitute<T>;
			libjaguar::Document doc;
			if(!obj) throw std::runtime_error("Invalid object pointer!");
			S sub(*obj);
			serialize(doc, Var(&sub));
			return doc;
		}

		///@cond
		template<Serializable T>
			requires std::is_enum_v<T>
		static libjaguar::Document toDocument(const T* obj) {
			libjaguar::Document doc;
			if(!obj) throw std::runtime_error("Invalid object pointer!");
			serialize(doc, Var(obj));
			return doc;
		}
		///@endcond

		/**
		 * @brief Serialize a T object to a vector of Jaguar data
		 *
		 * @tparam T The serializable type to serialize from
		 *
		 * @param obj The object to serialize
		 *
		 * @return A vector containing the serialized data encoded in Jaguar format
		 */
		template<Serializable T>
		static std::vector<uint8_t> toVector(const T* obj) {
			using S = SerializedSubstitute<T>;
			std::vector<uint8_t> result;
			if(!obj) throw std::runtime_error("Invalid object pointer!");
			S sub(*obj);
			serialize(result, Var(&sub));
			return result;
		}

		///@cond
		template<Serializable T>
			requires std::is_enum_v<T>
		static std::vector<uint8_t> toVector(const T* obj) {
			std::vector<uint8_t> result;
			if(!obj) throw std::runtime_error("Invalid object pointer!");
			serialize(result, Var(obj));
			return result;
		}
		///@endcond

		/**
		 * @brief Serialize a T object to a stream as Jaguar data
		 *
		 * @tparam T The serializable type to serialize from
		 *
		 * @param stream The stream to write the serialized Jaguar data to
		 * @param obj The object to serialize
		 */
		template<Serializable T>
		static void toStream(std::ostream& stream, const T* obj) {
			toDocument<T>(obj).ExportTo(stream);
		}

		/**
		 * @brief Serialize a Var to a Jaguar document
		 *
		 * @param var A Var holding the object to serialize
		 *
		 * @return A document containing the serialized data
		 */
		static libjaguar::Document toDocumentFromVar(Var var);

		/**
		 * @brief Serialize a Var to a vector of Jaguar data
		 *
		 * @param var A Var holding the object to serialize
		 *
		 * @return A vector containing the serialized data encoded in Jaguar format
		 */
		static std::vector<uint8_t> toVectorFromVar(Var var);

		/**
		 * @brief Serialize a Var to a stream as Jaguar data
		 *
		 * @param stream The stream to write the serialized Jaguar data to
		 * @param var A Var holding the object to serialize
		 */
		static void toStreamFromVar(std::ostream& stream, Var var) {
			toDocumentFromVar(var).ExportTo(stream);
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