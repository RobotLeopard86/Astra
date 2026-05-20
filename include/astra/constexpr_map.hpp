#pragma once

#include <array>

#include "constexpr_sort.hpp"
#include "expected.hpp"
#include "tools/format.hpp"

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
	class ConstexprMap {
	  public:
		///a pair like struct with deleted default constructor
		struct Entry {
			constexpr Entry() = delete;
			constexpr Entry(T value, std::string_view name)
			  : value(value), name(name) {
			}

			T value;
			std::string_view name;
		};

		constexpr ConstexprMap(std::array<Entry, size>&& array)
		  : _data(array), _value_ptr {}, _name_ptr {} {
			for(auto i = 0; i < _data.size(); i++) {
				_value_ptr[i] = &_data[i];
				_name_ptr[i] = &_data[i];
			}

			sortValuePtr();
			sortNamePtr();
		}

		Expected<std::string_view> getName(T value) const {
			auto ptr = searchByValue(value, 0, size);

			if(ptr == nullptr) {
				return Error("Cannot find an entry");
			}

			return ptr->name;
		}

		Expected<T> getValue(std::string_view name) const {
			auto ptr = searchByName(name, 0, size);

			if(ptr == nullptr) {
				return Error(format("Cannot find the constant '{}'", name));
			}

			return ptr->value;
		}

		[[nodiscard]] decltype(auto) begin() const {
			return _data.begin();
		}

		[[nodiscard]] decltype(auto) end() const {
			return _data.end();
		}

	  private:
		std::array<Entry, size> _data;
		std::array<Entry*, size> _value_ptr;
		std::array<Entry*, size> _name_ptr;

		constexpr Entry* searchByValue(T value, std::size_t begin, std::size_t end) const {

			while(begin < end) {

				auto middleIdx = (begin + end) / 2;
				auto middleVal = _value_ptr[middleIdx]->value;

				if(value == middleVal) {
					return _value_ptr[middleIdx];
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
				auto middleVal = _name_ptr[middleIdx]->name;

				if(name == middleVal) {
					return _name_ptr[middleIdx];
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
			ConstexprSort::sort(_value_ptr.data(), _value_ptr.size(),//
				[](auto a, auto b) { return a->value > b->value; });
		}
		constexpr void sortNamePtr() {
			ConstexprSort::sort(_name_ptr.data(), _name_ptr.size(),//
				[](auto a, auto b) { return a->name > b->name; });
		}
	};

}
