#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>

#include "astra/names.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T>
	struct ASTRA_API CommonActions {
		static const std::string& typeName() {
			static auto name = Names<T>::get();
			return name;
		}

		static std::size_t sizeOf() {
			return sizeof(T);
		}

		static void construct(void* p) {
			new(p) T();
		}

		static void destroy(void* p) {
			static_cast<T*>(p)->~T();
		}

		static void nop(void*) {
		}

		static void copy(void* to, const void* from) {
			if constexpr(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>) {
				*static_cast<T*>(to) = *static_cast<const T*>(from);
			} else {
				throw std::runtime_error("Cannot copy a non-copyable class!");
			}
		}

		static void move(void* to, void* from) {
			if constexpr(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>) {
				*static_cast<T*>(to) = std::move(*static_cast<T*>(from));
			} else {
				throw std::runtime_error("Cannot move a non-movable class!");
			}
		}
	};

}

///@endcond
