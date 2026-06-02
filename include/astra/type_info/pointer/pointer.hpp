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
			new(mem) StdSharedPtr<T>(value, isConst);
		}

		template<typename T>
		Pointer(std::unique_ptr<T>* value, bool isConst) {
			new(mem) StdUniquePtr<T>(value, isConst);
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
			impl()->unsafeAssign(ptr);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
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
		char mem[Sizeof<StdUniquePtr<int>, StdSharedPtr<int>>::max()];

		inline const IPointer* impl() const {
			return reinterpret_cast<const IPointer*>(&mem[0]);
		}

		inline IPointer* impl() {
			return reinterpret_cast<IPointer*>(&mem[0]);
		}
	};

}
