#pragma once

#include "astra/sizeof.hpp"
#include "astra/dll.hpp"
#include "doubleendedlist/std_deque.hpp"
#include "doubleendedlist/std_list.hpp"
#include "queue/std_queue.hpp"
#include "set/std_set.hpp"
#include "set/std_unordered_set.hpp"
#include "stack/std_stack.hpp"
#include "vector/std_vector.hpp"

namespace astra {
	/**
	 * @brief Reflection interface for a resizable container
	 */
	class ASTRA_API List {
	  public:
		///@cond
		List() = delete;
		~List() {
			impl()->~IList();
		}
		///@endcond

		template<typename T>
		List(std::vector<T>* vector, bool isConst) {
			new(mem) StdVector<T>(vector, isConst);
		}

		template<typename T>
		List(std::list<T>* list, bool isConst) {
			new(mem) StdList<T>(list, isConst);
		}

		template<typename T>
		List(std::deque<T>* deque, bool isConst) {
			new(mem) StdDeque<T>(deque, isConst);
		}

		template<typename T>
		List(std::stack<T>* stack, bool isConst) {
			new(mem) StdStack<T>(stack, isConst);
		}

		template<typename T>
		List(std::queue<T>* queue, bool isConst) {
			new(mem) StdQueue<T>(queue, isConst);
		}

		template<typename T>
		List(std::set<T>* set, bool isConst) {
			new(mem) StdSet<T>(set, isConst);
		}

		template<typename T>
		List(std::unordered_set<T>* set, bool isConst) {
			new(mem) StdUnorderedSet<T>(set, isConst);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
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
		 * @brief Get the TypeId of the elements in the list
		 *
		 * @return The contained element type ID
		 */
		TypeId nestedType() const {
			return impl()->nestedType();
		}

		/**
		 * @brief Execute a callback on every list element
		 */
		void forEach(std::function<void(Var)> callback) const {
			impl()->forEach(callback);
		}

		/**
		 * @brief Unsafely execute a callback on every list element
		 */
		void unsafeForEach(std::function<void(void*)> callback) const {
			impl()->unsafeForEach(callback);
		}

		/**
		 * @brief Clear the whole list
		 */
		void clear() {
			impl()->clear();
		}

		/**
		 * @brief Get the current number of elements in the list
		 *
		 * @return The number of list elements
		 */
		std::size_t size() const {
			return impl()->size();
		}

		/**
		 * @brief Push a new value into the list
		 *
		 * @param value The value to store
		 */
		void push(Var value) {
			return impl()->push(value);
		}

	  private:
		char mem[Sizeof<StdVector<int>, StdList<int>, StdDeque<int>, StdStack<int>,
			StdQueue<int>, StdSet<int>, StdUnorderedSet<int>>::max()];

		inline const IList* impl() const {
			return reinterpret_cast<const IList*>(&mem[0]);
		}

		inline IList* impl() {
			return reinterpret_cast<IList*>(&mem[0]);
		}
	};

}