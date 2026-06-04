#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "format.hpp"
#include "traits.hpp"
#include "dll.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#elif defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dbghelp.h>
#endif

namespace astra {
	/**
	 * @brief Helper class to get type names
	 *
	 * @tparam T The type whose name to get
	 */
	template<typename T>
	struct ASTRA_API Names {
		/**
		 * @brief Get the name of the type (will attempt to demangle if possible)
		 *
		 * @return The name of the type
		 */
		static std::string get() {
#ifdef __GNUG__
			int status = 1;
			return abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
#elif defined(_MSC_VER)
			std::string out;
			out.reserve(2048);
			DWORD result = UnDecorateSymbolName(typeid(T).name(), out.data(), out.size(), UNDNAME_COMPLETE);
			if(result == 0) return typeid(T).name();
			out.shrink_to_fit();
			return out;
#else
			return typeid(T).name();
#endif
		}
	};

	///@cond
	template<typename T, std::size_t size_v>
	struct Names<T[size_v]> {
		static std::string get() {
			static auto name = ::astra::format("{}[{}]", Names<T>::get(), size_v);
			return name;
		}
	};

	template<typename T, std::size_t size_v>
	struct Names<std::array<T, size_v>> {
		static std::string get() {
			static auto name = ::astra::format("std::array<{}, {}>", Names<T>::get(), size_v);
			return name;
		}
	};

	template<>
	struct Names<std::string> {
		static std::string get() {
			return "std::string";
		}
	};

	template<>
	struct Names<std::string_view> {
		static std::string get() {
			return "std::string_view";
		}
	};

	template<typename T>
	struct Names<std::vector<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::vector<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct Names<std::list<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::list<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct Names<std::deque<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::deque<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct Names<std::stack<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::stack<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct Names<std::queue<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::queue<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct Names<std::set<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::set<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename T>
	struct Names<std::unordered_set<T>> {
		static std::string get() {
			static auto name = ::astra::format("std::unordered_set<{}>", Names<T>::get());
			return name;
		}
	};

	template<typename K, typename V>
	struct Names<std::map<K, V>> {
		static std::string get() {
			static auto name = ::astra::format("std::map<{}, {}>", Names<K>::get(), Names<V>::get());
			return name;
		}
	};

	template<typename K, typename V>
	struct Names<std::unordered_map<K, V>> {
		static std::string get() {
			static auto name = ::astra::format("std::unordered_map<{}, {}>", Names<K>::get(), Names<V>::get());
			return name;
		}
	};
	///@endcond
}
