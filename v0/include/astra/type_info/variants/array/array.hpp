#pragma once

#include <memory>
#include <utility>

#include "c_array.hpp"
#include "astra/tools/sizeof.hpp"
#include "iarray.hpp"
#include "std_array.hpp"

namespace astra {

	struct Array final {
		Array() = delete;

		template<typename T, std::size_t size_v>
		Array(T (*array)[size_v], bool isConst) {
			new(_mem) CArray<T, size_v>(array, isConst);
		}

		template<typename T, std::size_t size_v>
		Array(std::array<T, size_v>* array, bool isConst) {
			new(_mem) StdArray<T, size_v>(array, isConst);
		}

		~Array() {
			impl()->~IArray();
		}

		Expected<None> assign(Var var) {
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

		Expected<Var> at(std::size_t idx) {
			return impl()->at(idx);
		}

		Expected<Var> operator[](std::size_t idx) {
			return impl()->operator[](idx);
		}

		Expected<Var> front() {
			return impl()->front();
		};

		Expected<Var> back() {
			return impl()->back();
		};

		Expected<None> fill(Var filler) {
			return impl()->fill(filler);
		}

		void forEach(std::function<void(Var)> callback) const {
			impl()->forEach(std::move(callback));
		}

		void forEach(std::function<void(Var)> callback) {
			impl()->forEach(std::move(callback));
		}

		void unsafeForEach(std::function<void(void*)> callback) const {
			impl()->unsafeForEach(std::move(callback));
		}

		std::size_t size() const {
			return impl()->size();
		}

	  private:
		char _mem[Sizeof<CArray<int, 1>, StdArray<int, 1>>::max()];

		inline const IArray* impl() const {
			return reinterpret_cast<const IArray*>(&_mem[0]);
		}

		inline IArray* impl() {
			return reinterpret_cast<IArray*>(&_mem[0]);
		}
	};

}
