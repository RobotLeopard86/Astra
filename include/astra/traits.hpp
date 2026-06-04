#pragma once

#include <array>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dll.hpp"

///@cond
namespace astra {

	template<typename T>
	struct is_ref_type : std::false_type {};

	template<typename T>
	inline constexpr bool is_ref_type_v = is_ref_type<T>::value;

	template<typename T>
	struct is_ref_type<std::basic_string_view<T>> : std::true_type {};

	template<>
	struct is_ref_type<const char*> : std::true_type {};

	template<>
	struct is_ref_type<const wchar_t*> : std::true_type {};

	template<typename T>
	struct is_string : std::false_type {};

	template<typename T>
	inline constexpr bool is_string_v = is_string<T>::value;

	template<typename T>
	struct is_string<std::basic_string<T>> : std::true_type {};

	template<typename T>
	struct is_string<std::basic_string_view<T>> : std::true_type {};

	template<>
	struct is_string<const char*> : std::true_type {};

	template<>
	struct is_string<const wchar_t*> : std::true_type {};

	template<typename T>
	struct is_list : std::false_type {};

	template<typename T>
	inline constexpr bool is_list_v = is_list<T>::value;

	template<typename T>
	struct is_list<std::vector<T>> : std::true_type {};

	template<typename T>
	struct is_list<std::list<T>> : std::true_type {};

	template<typename T>
	struct is_list<std::deque<T>> : std::true_type {};

	template<typename T>
	struct is_list<std::queue<T>> : std::true_type {};

	template<typename T>
	struct is_list<std::stack<T>> : std::true_type {};

	template<typename T>
	struct is_list<std::set<T>> : std::true_type {};

	template<typename T>
	struct is_list<std::unordered_set<T>> : std::true_type {};

	template<typename T>
	struct is_map : std::false_type {};

	template<typename T>
	inline constexpr bool is_map_v = is_map<T>::value;

	template<typename K, typename V>
	struct is_map<std::map<K, V>> : std::true_type {};

	template<typename K, typename V>
	struct is_map<std::unordered_map<K, V>> : std::true_type {};

	template<typename T>
	struct is_array {
		static constexpr bool value = std::is_array_v<T>;
	};

	template<typename T>
	inline constexpr bool is_array_v = is_array<T>::value;

	template<class T>
	using array_value_t = std::remove_reference_t<decltype(std::declval<T>()[0])>;

	template<typename T>
	struct is_class {
		static constexpr bool value =
			std::is_class_v<T> && !is_list_v<T> && !is_map_v<T> && !is_string_v<T>;
	};

	template<typename T>
	inline constexpr bool is_class_v = is_class<T>::value;

	template<typename T>
	struct is_std_array : std::false_type {};

	template<typename T>
	inline constexpr bool is_std_array_v = is_std_array<T>::value;

	template<typename T, std::size_t size_v>
	struct is_std_array<std::array<T, size_v>> : std::true_type {};

	template<typename T>
	struct is_shared_ptr : std::false_type {};

	template<typename T>
	inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

	template<typename T>
	struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

	template<typename T>
	struct is_unique_ptr : std::false_type {};

	template<typename T>
	inline constexpr bool is_unique_ptr_v = is_unique_ptr<T>::value;

	template<typename T>
	struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
}
///@endcond
