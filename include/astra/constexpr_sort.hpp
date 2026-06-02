#pragma once

#include <sys/types.h>
#include <algorithm>
#include <cstddef>
#include <type_traits>

#include "dll.hpp"

namespace astra {
	/**
	 * @brief Helper for sorting ConstexprMap objects (uses quicksort)
	 */
	struct ASTRA_API ConstexprSort {
		/**
		 * @brief Sort a given array in-place at compile-time
		 *
		 * @tparam T The type of the contained elements in the array
		 *
		 * @param arr The array to sort
		 * @param size The size of the array
		 */
		template<typename T>
		static constexpr void sort(std::remove_cvref_t<T>* arr, std::size_t size) {
			sort<T>(arr, size, [](std::add_const_t<std::remove_cvref_t<T>>& a, std::add_const_t<std::remove_cvref_t<T>>& b) { return a > b; });
		}

		/**
		 * @brief Sort a given array in-place at compile-time using a custom predicate
		 *
		 * @tparam T The type of the contained elements in the array
		 * @tparam L The type of the custom predicate
		 *
		 * @param arr The array to sort
		 * @param size The size of the array
		 * @param greater A custom predicate that returns true when element A is greater than element B
		 */
		template<typename T, typename L>
			requires std::is_invocable_v<L, std::add_const_t<std::remove_cvref_t<T>>&, std::add_const_t<std::remove_cvref_t<T>>&>
		static constexpr void sort(std::remove_cvref_t<T>* arr, std::size_t size, L greater) {
			quickSort<T, L>(arr, size, greater);
		}

		///@cond
		//Complexity O(log n)
		template<typename T>
		static constexpr void quickSort(std::remove_cvref_t<T>* arr, std::size_t size) {
			quickSort<T>(arr, size, [](std::add_const_t<std::remove_cvref_t<T>>& a, std::add_const_t<std::remove_cvref_t<T>>& b) { return a > b; });
		}

		//Complexity O(log n)
		template<typename T, typename L>
			requires std::is_invocable_v<L, std::add_const_t<std::remove_cvref_t<T>>&, std::add_const_t<std::remove_cvref_t<T>>&>
		static constexpr void quickSort(std::remove_cvref_t<T>* arr, std::size_t size, L greater) {
			quickSortReq<T, L>(arr, 0, size - 1, greater);
		}
		///@endcond

	  private:
		template<typename T, typename L>
			requires std::is_invocable_v<L, std::add_const_t<std::remove_cvref_t<T>>&, std::add_const_t<std::remove_cvref_t<T>>&>
		static constexpr void quickSortReq(std::remove_cvref_t<T>* arr, std::size_t begin, std::size_t end, L greater) {
			if(begin >= end) {
				return;
			}
			auto pivot = hoare<T, L>(arr, begin, end, greater);

			quickSortReq<T, L>(arr, begin, pivot, greater);
			quickSortReq<T, L>(arr, pivot + 1, end, greater);
		}

		template<typename T, typename L>
			requires std::is_invocable_v<L, std::add_const_t<std::remove_cvref_t<T>>&, std::add_const_t<std::remove_cvref_t<T>>&>
		static constexpr std::size_t hoare(std::remove_cvref_t<T>* arr, std::size_t begin, std::size_t end, L greater) {
			auto pivot = arr[(begin + end) / 2];

			auto i = begin - 1;
			auto j = end + 1;

			while(true) {
				do {
					i++;
				} while(greater(pivot, arr[i]));

				do {
					j--;
				} while(greater(arr[j], pivot));

				if(i >= j) {
					return j;
				}

				std::swap(arr[i], arr[j]);
			}
		}
	};
}