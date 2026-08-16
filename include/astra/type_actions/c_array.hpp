#pragma once

#include <cstddef>
#include <cstring>

#include "astra/type_table.hpp"
#include "astra/traits.hpp"
#include "astra/type_actions/common_actions.hpp"
#include "astra/type_actions/type_actions.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	template<typename T, std::size_t size_v>
	struct TypeActions<T[size_v]> {

		static TypeInfo reflect(void* value, bool isConst) {
			return TypeInfo(Array(static_cast<T(*)[size_v]>(value), isConst));
		}

		static std::size_t sizeOf() {
			return sizeof(T[size_v]);
		}

		static void construct(void* p) {
			if constexpr(!std::is_fundamental_v<T>) {
				new(p) T[size_v];
			}
			//do nothing for fundamental
		}

		static void destroy(void* p) {
			if constexpr(!std::is_fundamental_v<T>) {
				for(auto i = 0; i < size_v; i++) {
					static_cast<T*>(p)[i].~T();
				}
			}
			//do nothing for fundamental
		}

		static void copy(void* to, const void* from) {
			if constexpr(std::is_fundamental_v<T>) {
				std::memcpy(to, from, size_v);
			} else {
				for(auto i = 0; i < size_v; i++) {
					static_cast<T*>(to)[i] = static_cast<T*>(from)[i];
				}
			}
		}

		static void move(void* to, void* from) {
			if constexpr(std::is_fundamental_v<T>) {
				//copy fundamental types anyway
				std::memcpy(to, from, size_v);
			} else {
				for(auto i = 0; i < size_v; i++) {
					static_cast<T*>(to)[i] = std::move(static_cast<T*>(from)[i]);
				}
			}
		}
	};

	template<typename T>
		requires is_array_v<T>
	TypeId TypeId::get() {
		static TypeId id(TypeTable::recordActions(Actions(&TypeActions<T>::reflect,
			&CommonActions<T>::typeName,
			&TypeActions<T>::sizeOf,
			&TypeActions<T>::construct,
			&TypeActions<T>::destroy,
			&TypeActions<T>::copy,
			&TypeActions<T>::move)));
		return id;
	}

}

///@endcond
