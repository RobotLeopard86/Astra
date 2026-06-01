#pragma once

#include <memory>
#include <utility>

#include "astra/dll.hpp"
#include "c_array.hpp"
#include "astra/sizeof.hpp"
#include "iarray.hpp"
#include "std_array.hpp"

namespace astra {

	struct ASTRA_API Array final {
		Array() = delete;

		template<typename T, std::size_t size_v>
		Array(T (*array)[size_v], bool isConst) {
			new(mem) CArray<T, size_v>(array, isConst);
		}

		template<typename T, std::size_t size_v>
		Array(std::array<T, size_v>* array, bool isConst) {
			new(mem) StdArray<T, size_v>(array, isConst);
		}

		~Array() {
			impl()->~IArray();
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

		Var at(std::size_t idx) {
			return impl()->at(idx);
		}

		Var operator[](std::size_t idx) {
			return impl()->operator[](idx);
		}

		Var front() {
			return impl()->front();
		};

		Var back() {
			return impl()->back();
		};

		void fill(Var filler) {
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
		char mem[Sizeof<CArray<int, 1>, StdArray<int, 1>>::max()];

		inline const IArray* impl() const {
			return reinterpret_cast<const IArray*>(&mem[0]);
		}

		inline IArray* impl() {
			return reinterpret_cast<IArray*>(&mem[0]);
		}
	};

}
