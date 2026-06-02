#pragma once

#include <array>

#include "constexpr_sort.hpp"
#include "format.hpp"
#include "dll.hpp"

namespace astra {
	/**
	 * @brief A tree-based map that works at compile-time
	 * @details The map is sorted and thus has O(log n) search time complexity, using separate pointers for keys and values; O(n^2) complexity on initial sort
	 *
	 * @tparam T The type of data to store
	 * @tparam size The size of the map
	 */
	template<class T, std::size_t size>
	class ASTRA_API ConstexprMap {
	  public:
		/**
		 * @brief Storage for a given map entry
		 */
		struct ASTRA_API Entry {
			constexpr Entry() = delete;
			constexpr Entry(T value, std::string_view name)
			  : value(value), name(name) {
			}

			T value;		 ///<The contained value
			std::string name;///<The associated string key
		};

		/**
		 * @brief Create a new map from array data
		 *
		 * @param array The array of key-value pairs to convert into a map
		 */
		constexpr ConstexprMap(std::array<Entry, size>&& array)
		  : data(array), value_ptr {}, name_ptr {} {
			for(auto i = 0; i < data.size(); i++) {
				value_ptr[i] = &data[i];
				name_ptr[i] = &data[i];
			}

			sortValuePtr();
			sortNamePtr();
		}

		/**
		 * @brief Get the key of a value in the map
		 *
		 * @param value The value corresponding to the desired key
		 *
		 * @return The value's key
		 *
		 * @throws std::runtime_error If no entry with the given value exists
		 */
		const std::string& getKey(T value) const {
			auto ptr = searchByValue(value, 0, size);

			if(ptr == nullptr) {
				throw std::runtime_error("Cannot find an entry");
			}

			return ptr->name;
		}

		/**
		 * @brief Get the value associated with a key in the map
		 *
		 * @param key The key corresponding to the desired value
		 *
		 * @return The value referenced by the key
		 *
		 * @throws std::runtime_error If no entry with the given key exists
		 */
		T getValue(const std::string& key) const {
			auto ptr = searchByKey(key, 0, size);

			if(ptr == nullptr) {
				throw std::runtime_error(::astra::format("Cannot find the key \"{}\"", key));
			}

			return ptr->value;
		}

		/**
		 * @brief Get an iterator pointing to the start of the container
		 *
		 * @return The iterator
		 */
		[[nodiscard]] decltype(auto) begin() const {
			return data.begin();
		}

		/**
		 * @brief Get an iterator pointing to the end of the container
		 *
		 * @return The iterator
		 */
		[[nodiscard]] decltype(auto) end() const {
			return data.end();
		}

	  private:
		std::array<Entry, size> data;
		std::array<Entry*, size> value_ptr;
		std::array<Entry*, size> name_ptr;

		//Binary search algorithm
		constexpr Entry* searchByValue(T value, std::size_t begin, std::size_t end) const {
			while(begin < end) {
				auto middleIdx = (begin + end) / 2;
				auto middleVal = value_ptr[middleIdx]->value;
				if(value == middleVal) {
					return value_ptr[middleIdx];
				}
				if(value < middleVal) {
					end = middleIdx;
				} else {
					begin = ++middleIdx;
				}
			}

			//Fallback nullptr return
			return nullptr;
		}

		//Binary search algorithm
		constexpr Entry* searchByKey(std::string_view name, std::size_t begin, std::size_t end) const {
			while(begin < end) {
				auto middleIdx = (begin + end) / 2;
				auto middleVal = name_ptr[middleIdx]->name;
				if(name == middleVal) {
					return name_ptr[middleIdx];
				}
				if(name < middleVal) {
					end = middleIdx;
				} else {
					begin = ++middleIdx;
				}
			}

			//Fallback nullptr return
			return nullptr;
		}

		constexpr void sortValuePtr() {
			ConstexprSort::sort<Entry*>(value_ptr.data(), value_ptr.size(),
				[](auto a, auto b) { return a->value > b->value; });
		}
		constexpr void sortNamePtr() {
			ConstexprSort::sort<Entry*>(name_ptr.data(), name_ptr.size(),
				[](auto a, auto b) { return a->name > b->name; });
		}
	};

}
