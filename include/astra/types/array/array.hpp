#pragma once

#include <utility>

#include "astra/dll.hpp"
#include "c_array.hpp"
#include "astra/sizeof.hpp"
#include "iarray.hpp"
#include "std_array.hpp"

namespace astra {
	/**
	 * @brief Reflection interface for a fixed-size container
	 */
	class ASTRA_API Array final {
	  public:
		///@cond
		Array() = delete;
		///@endcond

		/**
		 * @brief Create an Array from a raw C array
		 *
		 * @tparam T The type of elements in the array
		 * @tparam size_v The size of the array
		 *
		 * @param array The array to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename T, std::size_t size_v>
		Array(T (*array)[size_v], bool isConst) {
			new(mem) CArray<T, size_v>(array, isConst);
		}

		/**
		 * @brief Create an Array from a @c std::array
		 *
		 * @tparam T The type of elements in the array
		 * @tparam size_v The size of the array
		 *
		 * @param array The array to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename T, std::size_t size_v>
		Array(std::array<T, size_v>* array, bool isConst) {
			new(mem) StdArray<T, size_v>(array, isConst);
		}

		///@cond
		~Array() {
			impl()->~IArray();
		}
		///@endcond

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a array
		 */
		void assign(Var var) {
			return impl()->assign(var);
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) {
			return impl()->unsafeAssign(ptr);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var ownVar() const {
			return impl()->ownVar();
		}

		/**
		 * @brief Get the TypeId of the elements in the array
		 *
		 * @return The contained element type ID
		 */
		TypeId nestedType() const {
			return impl()->nestedType();
		}

		/**
		 * @brief Access the contents of an array element as a Var
		 *
		 * @param idx The index of the element to access
		 *
		 * @return A Var holding the contents of the specified array element, const state inherited
		 */
		Var at(std::size_t idx) {
			return impl()->at(idx);
		}

		/**
		 * @brief Access the contents of an array element as a Var
		 *
		 * @param idx The index of the element to access
		 *
		 * @return A Var holding the contents of the specified array element, const state inherited
		 */
		Var operator[](std::size_t idx) {
			return impl()->operator[](idx);
		}

		/**
		 * @brief Access the contents of the first array element as a Var
		 *
		 * @return A Var holding the contents of the first array element, const state inherited
		 */
		Var front() {
			return impl()->front();
		};

		/**
		 * @brief Access the contents of the last array element as a Var
		 *
		 * @return A Var holding the contents of the last array element, const state inherited
		 */
		Var back() {
			return impl()->back();
		};

		/**
		 * @brief Replace every element in the container with copies of the provided Var
		 */
		void fill(Var filler) {
			return impl()->fill(filler);
		}

		/**
		 * @brief Constly execute a callback on every array element
		 *
		 * @param callback The callback to execute
		 */
		void forEach(std::function<void(Var)> callback) const {
			impl()->forEach(std::move(callback));
		}

		/**
		 * @brief Execute a callback on every array element
		 *
		 * @param callback The callback to execute
		 */
		void forEach(std::function<void(Var)> callback) {
			impl()->forEach(std::move(callback));
		}

		/**
		 * @brief Unsafely execute a callback on every array element
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param callback The callback to execute
		 */
		void unsafeForEach(std::function<void(void*)> callback) const {
			impl()->unsafeForEach(std::move(callback));
		}

		/**
		 * @brief Get the number of elements in the array
		 *
		 * @return The number of array elements
		 */
		std::size_t size() const {
			return impl()->size();
		}

	  private:
		char mem[Sizeof<CArray<int, 1>, StdArray<int, 1>>::max()];

		inline const IArray* impl() const {
			return reinterpret_cast<const IArray*>(&mem[0]);
		}

		inline IArray* impl() {
			return reinterpret_cast<IArray*>(&mem[0]);
		}
	};

}
