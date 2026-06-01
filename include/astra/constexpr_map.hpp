#pragma once

#include <array>

#include "constexpr_sort.hpp"
#include "format.hpp"
#include "dll.hpp"

namespace astra {

	///the analogue of tree based map container with fixed type of second argument
	///and the same log(N) search performance
	///it contains the same number of data: value and twe pointers
	///but in this case all of them stored in different arrays and arrays for
	///T values and names are sorted
	///which could guarantee log(N) performance with binary search
	///for both value->name and name->value access
	///
	///initial compile time sorting has O(N^2) complexity
	template<class T, std::size_t size>
	class ASTRA_API ConstexprMap {
	  public:
		///a pair like struct ASTRA_API with deleted default constructor
		struct ASTRA_API Entry {
			constexpr Entry() = delete;
			constexpr Entry(T value, std::string_view name)
			  : value(value), name(name) {
			}

			T value;
			std::string name;
		};

		constexpr ConstexprMap(std::array<Entry, size>&& array)
		  : data(array), value_ptr {}, name_ptr {} {
			for(auto i = 0; i < data.size(); i++) {
				value_ptr[i] = &data[i];
				name_ptr[i] = &data[i];
			}

			sortValuePtr();
			sortNamePtr();
		}

		const std::string& getName(T value) const {
			auto ptr = searchByValue(value, 0, size);

			if(ptr == nullptr) {
				throw std::runtime_error("Cannot find an entry");
			}

			return ptr->name;
		}

		T getValue(const std::string& name) const {
			auto ptr = searchByName(name, 0, size);

			if(ptr == nullptr) {
				throw std::runtime_error(::astra::format("Cannot find the constant '{}'", name));
			}

			return ptr->value;
		}

		[[nodiscard]] decltype(auto) begin() const {
			return data.begin();
		}

		[[nodiscard]] decltype(auto) end() const {
			return data.end();
		}

	  private:
		std::array<Entry, size> data;
		std::array<Entry*, size> value_ptr;
		std::array<Entry*, size> name_ptr;

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

			//nothing is found
			return nullptr;
		}

		constexpr Entry* searchByName(std::string_view name, std::size_t begin, std::size_t end) const {

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

			//nothing is found
			return nullptr;
		}

		constexpr void sortValuePtr() {
			ConstexprSort::sort(value_ptr.data(), value_ptr.size(),//
				[](auto a, auto b) { return a->value > b->value; });
		}
		constexpr void sortNamePtr() {
			ConstexprSort::sort(name_ptr.data(), name_ptr.size(),//
				[](auto a, auto b) { return a->name > b->name; });
		}
	};

}
