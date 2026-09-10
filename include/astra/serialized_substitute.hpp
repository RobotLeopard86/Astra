#pragma once

#include "type_actions/type_actions.hpp"

#include <vector>
#include <array>
#include <deque>
#include <map>
#include <unordered_map>
#include <ranges>

///@cond
#define ASTRA_SUBSTITUTE_SERIALIZE(type)               \
	void ASTRA__serializeinternal(const void* in) {    \
		this->serialize(static_cast<const type*>(in)); \
	}                                                  \
	SerializedSubstitute(const type& input) {          \
		this->serialize(&input);                       \
	}                                                  \
	void serialize(const type* in)

#define ASTRA_SUBSTITUTE_DESERIALIZE(type)             \
	void ASTRA__deserializeinternal(void* out) const { \
		this->deserialize(static_cast<type*>(out));    \
	}                                                  \
	void deserialize(type* out) const
///@endcond

namespace astra {
	/**
	 * @brief Template for specifying a substitute reflectable type for a non-reflectable type T.
	 *
	 * The default specialization is marked as invalid. To provide a substitute, specialize this template for your type T.
	 */
	template<typename T>
	struct SerializedSubstitute {
		static constexpr bool invalid = true;
	};

	///@cond
	struct TypeId;

	template<typename U>
	struct SerializedSubstitute<std::vector<U>> : public std::vector<SerializedSubstitute<U>> {
	  public:
		SerializedSubstitute() = default;

		ASTRA_SUBSTITUTE_SERIALIZE(std::vector<U>) {
			auto transformed = (*in) | std::views::transform([](const U& item) {
				return SerializedSubstitute<U>(item);
			}) | std::views::common;
			this->assign(transformed.begin(), transformed.end());
		}

		ASTRA_SUBSTITUTE_DESERIALIZE(std::vector<U>) {
			out->resize(this->size());
			for(std::size_t i = 0; i < this->size(); ++i) {
				(*this)[i].deserialize(&(*out)[i]);
			}
		}

		virtual ~SerializedSubstitute() {}
		friend struct TypeActions<SerializedSubstitute>;
		virtual TypeId ASTRA__gettypeid() const override;
	};

	template<typename U>
	struct SerializedSubstitute<std::deque<U>> : public std::vector<SerializedSubstitute<U>> {
	  public:
		SerializedSubstitute() = default;

		ASTRA_SUBSTITUTE_SERIALIZE(std::deque<U>) {
			auto transformed = (*in) | std::views::transform([](const U& item) {
				return SerializedSubstitute<U>(item);
			}) | std::views::common;
			this->assign(transformed.begin(), transformed.end());
		}

		ASTRA_SUBSTITUTE_DESERIALIZE(std::vector<U>) {
			out->resize(this->size());
			for(std::size_t i = 0; i < this->size(); ++i) {
				(*this)[i].deserialize(&(*out)[i]);
			}
		}

		virtual ~SerializedSubstitute() {}
		friend struct TypeActions<SerializedSubstitute>;
		virtual TypeId ASTRA__gettypeid() const override;
	};

	template<typename U, std::size_t C>
	struct SerializedSubstitute<std::array<U, C>> : public std::array<SerializedSubstitute<U>, C> {
	  public:
		SerializedSubstitute() = default;
		using arr_t = std::array<U, C>;

		ASTRA_SUBSTITUTE_SERIALIZE(arr_t) {
			auto transformed = (*in) | std::views::transform([](const U& item) {
				return SerializedSubstitute<U>(item);
			}) | std::views::common;
			this->assign(transformed.begin(), transformed.end());
		}

		ASTRA_SUBSTITUTE_DESERIALIZE(arr_t) {
			for(std::size_t i = 0; i < this->size(); ++i) {
				(*this)[i].deserialize(&(*out)[i]);
			}
		}

		virtual ~SerializedSubstitute() {}
		friend struct TypeActions<SerializedSubstitute>;
		virtual TypeId ASTRA__gettypeid() const override;
	};

	template<typename K, typename V>
	struct SerializedSubstitute<std::map<K, V>> : public std::map<SerializedSubstitute<K>, SerializedSubstitute<V>> {
	  public:
		SerializedSubstitute() = default;
		using map_t = std::map<K, V>;

		ASTRA_SUBSTITUTE_SERIALIZE(map_t) {
			auto transformed = (*in) | std::views::transform([](const std::pair<K, V>& item) {
				return std::make_pair<SerializedSubstitute<K>, SerializedSubstitute<V>>(item.first, item.second);
			}) | std::views::common;
			this->assign(transformed.begin(), transformed.end());
		}

		ASTRA_SUBSTITUTE_DESERIALIZE(map_t) {
			auto transformed = (*this) | std::views::transform([](const std::pair<SerializedSubstitute<K>, SerializedSubstitute<V>>& item) {
				K k;
				item.first.deserialize(&k);
				V v;
				item.second.deserialize(&v);
				return std::pair<K, V>(k, v);
			}) | std::views::common;
			out->assign(transformed.begin(), transformed.end());
		}

		virtual ~SerializedSubstitute() {}
		friend struct TypeActions<SerializedSubstitute>;
		virtual TypeId ASTRA__gettypeid() const override;
	};

	template<typename K, typename V>
	struct SerializedSubstitute<std::unordered_map<K, V>> : public std::map<SerializedSubstitute<K>, SerializedSubstitute<V>> {
	  public:
		SerializedSubstitute() = default;
		using map_t = std::unordered_map<K, V>;

		ASTRA_SUBSTITUTE_SERIALIZE(map_t) {
			auto transformed = (*in) | std::views::transform([](const std::pair<K, V>& item) {
				return std::make_pair<SerializedSubstitute<K>, SerializedSubstitute<V>>(item.first, item.second);
			}) | std::views::common;
			this->assign(transformed.begin(), transformed.end());
		}

		ASTRA_SUBSTITUTE_DESERIALIZE(map_t) {
			auto transformed = (*this) | std::views::transform([](const std::pair<SerializedSubstitute<K>, SerializedSubstitute<V>>& item) {
				K k;
				item.first.deserialize(&k);
				V v;
				item.second.deserialize(&v);
				return std::pair<K, V>(k, v);
			}) | std::views::common;
			out->assign(transformed.begin(), transformed.end());
		}

		virtual ~SerializedSubstitute() {}
		friend struct TypeActions<SerializedSubstitute>;
		virtual TypeId ASTRA__gettypeid() const override;
	};
	///@endcond
}