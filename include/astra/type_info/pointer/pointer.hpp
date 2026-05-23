#pragma once

#include "astra/sizeof.hpp"
#include "ipointer.hpp"
#include "std_shared_ptr.hpp"
#include "std_unique_ptr.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Pointer {
		template<typename T>
		Pointer(std::shared_ptr<T>* value, bool isConst) {
			new(_mem) StdSharedPtr<T>(value, isConst);
		}

		template<typename T>
		Pointer(std::unique_ptr<T>* value, bool isConst) {
			new(_mem) StdUniquePtr<T>(value, isConst);
		}

		void assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		Var var() const {
			return impl()->var();
		}

		bool isNull() const {
			return impl()->isNull();
		}

		void init() {
			impl()->init();
		}

		Var getNested() const {
			return impl()->getNested();
		}

	  private:
		char _mem[Sizeof<StdUniquePtr<int>, StdSharedPtr<int>>::max()];

		inline const IPointer* impl() const {
			return reinterpret_cast<const IPointer*>(&_mem[0]);
		}

		inline IPointer* impl() {
			return reinterpret_cast<IPointer*>(&_mem[0]);
		}
	};

}
