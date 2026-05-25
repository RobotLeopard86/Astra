#pragma once

#include <sys/types.h>
#include <algorithm>
#include <cstddef>

#include "dll.hpp"

struct ASTRA_API ConstexprSort {
	//Has optimized complexity
	template<typename T>
	static constexpr void sort(T* arr, std::size_t size) {
		sort(arr, size, [](T a, T b) { return a > b; });
	}

	//Has optimized complexity
	template<typename T, typename LambdaT>
	static constexpr void sort(T* arr, std::size_t size, LambdaT greater) {
		if(size > 32) {
			quickSort(arr, size, greater);
		} else {
			insertionSort(arr, size, greater);
		}
	}

	//Has complexity O(N^2)
	template<typename T>
	static constexpr void insertionSort(T* arr, std::size_t size) {
		insertionSort(arr, size, [](T a, T b) { return a > b; });
	}

	//Has complexity O(N^2)
	template<typename T, typename LambdaT>
	static constexpr void insertionSort(T* arr, std::size_t size, LambdaT comp) {
		for(auto i = 1; i < size; i++) {
			auto key = arr[i];

			auto j = i;
			while(j > 0 && comp(arr[j - 1], key)) {
				arr[j] = arr[j - 1];
				j--;
			}
			arr[j] = key;
		}
	}

	//Has complexity O(log(N))
	template<typename T>
	static constexpr void quickSort(T* arr, std::size_t size) {
		quickSort(arr, size, [](T a, T b) { return a > b; });
	}

	//Has complexity O(log(N))
	template<typename T, typename LambdaT>
	static constexpr void quickSort(T* arr, std::size_t size, LambdaT greater) {
		quickSortReq(arr, 0, size - 1, greater);
	}

  private:
	template<typename T, typename LambdaT>
	static constexpr void quickSortReq(T* arr, std::size_t begin, std::size_t end, LambdaT greater) {
		if(begin >= end) {
			return;
		}
		auto pivot = hoare(arr, begin, end, greater);

		quickSortReq(arr, begin, pivot, greater);
		quickSortReq(arr, pivot + 1, end, greater);
	}

	template<typename T, typename LambdaT>
	static constexpr std::size_t hoare(T* arr, std::size_t begin, std::size_t end, LambdaT greater) {
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
