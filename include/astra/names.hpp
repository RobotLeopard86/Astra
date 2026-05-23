#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "format.hpp"
#include "traits.hpp"
#include "dll.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace astra {

	template<typename T>
	struct ASTRA_API Names {
		static std::string_view get() {
#ifdef __GNUG__
			int status = -4;
			return abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
#else
			return typeid(T).name();
#endif
		}
	};

	template<typename T, std::size_t size_v>
	struct ASTRA_API Names<T[size_v]> {
		static std::string_view get() {
			static auto name = format("{}[{}]", Names<T>::get(), size_v);
			return name;
		}
	};

	template<typename T, std::size_t size_v>
	struct ASTRA_API Names<std::array<T, size_v>> {
		static std::string_view get() {
			static auto name = format("std::array<{}, {}>", Names<T>::get(), size_v);
			return name;
		}
	};

	template<>
	struct ASTRA_API Names<std::string> {
		static std::string_view get() {
			return "std::string";
		}
	};

	template<>
	struct ASTRA_API Names<std::string_view> {
		static std::string_view get() {
			return "std::string_view";
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::vector<T>> {
		static std::string_view get() {
			static auto name = format("std::vector<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::list<T>> {
		static std::string_view get() {
			static auto name = format("std::list<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::deque<T>> {
		static std::string_view get() {
			static auto name = format("std::deque<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::stack<T>> {
		static std::string_view get() {
			static auto name = format("std::stack<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::queue<T>> {
		static std::string_view get() {
			static auto name = format("std::queue<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::set<T>> {
		static std::string_view get() {
			static auto name = format("std::set<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct ASTRA_API Names<std::unordered_set<T>> {
		static std::string_view get() {
			static auto name = format("std::unordered_set<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename KeyT, typename ValueT>
	struct ASTRA_API Names<std::map<KeyT, ValueT>> {
		static std::string_view get() {
			static auto name = format("std::map<{}, {}>", Names<KeyT>::get(), Names<ValueT>::get());
			return name;
		}
	};

	template<typename KeyT, typename ValueT>
	struct ASTRA_API Names<std::unordered_map<KeyT, ValueT>> {
		static std::string_view get() {
			static auto name = format("std::unordered_map<{}, {}>", Names<KeyT>::get(), Names<ValueT>::get());
			return name;
		}
	};

}
