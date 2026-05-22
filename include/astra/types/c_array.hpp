#pragma once

#include <cstddef>
#include <cstring>

#include "astra/actions_table.hpp"
#include "astra/types/common_actions.hpp"
#include "astra/types/type_actions.hpp"

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

	template<typename T, std::size_t size_v>
	TypeId TypeId::get(T (*)[size_v]) {
		static TypeId id(ActionsTable::record(Actions(&TypeActions<T[size_v]>::reflect,//
			&CommonActions<T[size_v]>::typeName,									   //
			&TypeActions<T[size_v]>::sizeOf,										   //
			&TypeActions<T[size_v]>::construct,										   //
			&TypeActions<T[size_v]>::destroy,										   //
			&TypeActions<T[size_v]>::copy,											   //
			&TypeActions<T[size_v]>::move)));
		return id;
	}

}
