#pragma once

#include <array>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "astra/names.hpp"
#include "astra/traits.hpp"
#include "dll.hpp"

namespace astra {

	///copyable value type with single int under the hood
	///represent sequential type id
	struct ASTRA_API TypeId {
		TypeId()
		  : id(0) {
		}

		///proxy function for using with implicit type in TypeId::get<T>() way without argument
		template<typename T>
		static TypeId get() {
			return get(static_cast<T*>(nullptr));
		}

		///the main function of TypeId mechanism
		template<typename T>
			requires is_class_v<T>
		static TypeId get(T*) {
			return TypeId(0);
		}

		template<typename T>
			requires std::is_enum_v<T>
		static TypeId get(T*) {
			return TypeId(0);
		}

		template<typename T>
			requires std::is_void_v<T>
		static TypeId get(T*) {
			return TypeId(0);
		}

		///specializations defined in ./types
		template<typename T>
			requires std::is_integral_v<T>
		static TypeId get(T* ptr);

		template<typename T>
			requires std::is_floating_point_v<T>
		static TypeId get(T* ptr);

		template<typename T>
			requires is_string_v<T>
		static TypeId get(T* ptr);

		template<typename T, std::size_t size>
		static TypeId get(T (*array)[size]);

		template<typename T, std::size_t size>
		static TypeId get(std::array<T, size>* array);

		template<typename T>
			requires is_list_v<T>
		static TypeId get(T* ptr);

		template<typename T>
			requires is_map_v<T>
		static TypeId get(T* ptr);

		template<typename T>
		static TypeId get(std::unique_ptr<T>* ptr);

		template<typename T>
		static TypeId get(std::shared_ptr<T>* ptr);

		//other methods
		bool operator==(const TypeId& other) const {
			return id == other.id;
		}

		bool operator!=(const TypeId& other) const {
			return id != other.id;
		}

		uint32_t number() const {
			return id;
		}

	  private:
		uint32_t id;

		explicit TypeId(uint32_t id)
		  : id(id) {}
	};

}
