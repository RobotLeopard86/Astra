#pragma once

#include <array>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API is_ref_type : std::false_type {};

	template<typename T>
	inline constexpr bool is_ref_type_v = is_ref_type<T>::value;

	template<typename T>
	struct ASTRA_API is_ref_type<std::basic_string_view<T>> : std::true_type {};

	template<>
	struct ASTRA_API is_ref_type<const char*> : std::true_type {};

	template<>
	struct ASTRA_API is_ref_type<const wchar_t*> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_string : std::false_type {};

	template<typename T>
	inline constexpr bool is_string_v = is_string<T>::value;

	template<typename T>
	struct ASTRA_API is_string<std::basic_string<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_string<std::basic_string_view<T>> : std::true_type {};

	template<>
	struct ASTRA_API is_string<const char*> : std::true_type {};

	template<>
	struct ASTRA_API is_string<const wchar_t*> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list : std::false_type {};

	template<typename T>
	inline constexpr bool is_list_v = is_list<T>::value;

	template<typename T>
	struct ASTRA_API is_list<std::vector<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list<std::list<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list<std::deque<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list<std::queue<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list<std::stack<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list<std::set<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_list<std::unordered_set<T>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_map : std::false_type {};

	template<typename T>
	inline constexpr bool is_map_v = is_map<T>::value;

	template<typename KeyT, typename ValueT>
	struct ASTRA_API is_map<std::map<KeyT, ValueT>> : std::true_type {};

	template<typename KeyT, typename ValueT>
	struct ASTRA_API is_map<std::unordered_map<KeyT, ValueT>> : std::true_type {};

	template<typename T>
	struct ASTRA_API is_array {
		static constexpr bool value = std::is_array_v<T>;
	};

	template<typename T>
	inline constexpr bool is_array_v = is_array<T>::value;

	template<typename T, std::size_t size>
	struct ASTRA_API is_array<std::array<T, size>> : std::true_type {};

	template<class T>
	using array_value_t = std::remove_reference_t<decltype(std::declval<T>()[0])>;

	template<typename T>
	struct ASTRA_API is_class {
		static constexpr bool value =
			std::is_class_v<T> && !is_list_v<T> && !is_map_v<T> && !is_string_v<T>;
	};

	template<typename T>
	inline constexpr bool is_class_v = is_class<T>::value;

}
