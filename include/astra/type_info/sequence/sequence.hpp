#pragma once

#include "astra/sizeof.hpp"
#include "list/std_deque.hpp"
#include "list/std_list.hpp"
#include "queue/std_queue.hpp"
#include "set/std_set.hpp"
#include "set/std_unordered_set.hpp"
#include "stack/std_stack.hpp"
#include "vector/std_vector.hpp"

namespace astra {

	struct Sequence {
		Sequence() = delete;

		template<typename T>
		Sequence(std::vector<T>* vector, bool isConst) {
			new(_mem) StdVector<T>(vector, isConst);
		}

		template<typename T>
		Sequence(std::list<T>* list, bool isConst) {
			new(_mem) StdList<T>(list, isConst);
		}

		template<typename T>
		Sequence(std::deque<T>* deque, bool isConst) {
			new(_mem) StdDeque<T>(deque, isConst);
		}

		template<typename T>
		Sequence(std::stack<T>* stack, bool isConst) {
			new(_mem) StdStack<T>(stack, isConst);
		}

		template<typename T>
		Sequence(std::queue<T>* queue, bool isConst) {
			new(_mem) StdQueue<T>(queue, isConst);
		}

		template<typename T>
		Sequence(std::set<T>* set, bool isConst) {
			new(_mem) StdSet<T>(set, isConst);
		}

		template<typename T>
		Sequence(std::unordered_set<T>* set, bool isConst) {
			new(_mem) StdUnorderedSet<T>(set, isConst);
		}

		~Sequence() {
			impl()->~ISequence();
		}

		void assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			return impl()->unsafeAssign(ptr);
		}

		Var ownVar() const {
			return impl()->ownVar();
		}

		TypeId nestedType() const {
			return impl()->nestedType();
		}

		void forEach(std::function<void(Var)> callback) const {
			impl()->forEach(callback);
		}

		void unsafeForEach(std::function<void(void*)> callback) const {
			impl()->unsafeForEach(callback);
		}

		void clear() {
			impl()->clear();
		}

		std::size_t size() const {
			return impl()->size();
		}

		void push(Var value) {
			return impl()->push(value);
		}

	  private:
		char _mem[Sizeof<StdVector<int>, StdList<int>, StdDeque<int>, StdStack<int>,//
			StdQueue<int>, StdSet<int>, StdUnorderedSet<int>>::max()];

		inline const ISequence* impl() const {
			return reinterpret_cast<const ISequence*>(&_mem[0]);
		}

		inline ISequence* impl() {
			return reinterpret_cast<ISequence*>(&_mem[0]);
		}
	};

}
