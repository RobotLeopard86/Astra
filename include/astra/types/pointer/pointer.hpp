#pragma once

#include "astra/sizeof.hpp"
#include "ipointer.hpp"
#include "std_shared_ptr.hpp"
#include "std_unique_ptr.hpp"
#include "astra/dll.hpp"

namespace astra {

	class ASTRA_API Pointer {
	  public:
		/**
		 * @brief Create a new pointer from a @c std::shared_ptr
		 *
		 * @tparam T The type of the contained object
		 *
		 * @param ptr The pointer to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename T>
		Pointer(std::shared_ptr<T>* ptr, bool isConst) {
			new(mem) StdSharedPtr<T>(ptr, isConst);
		}

		/**
		 * @brief Create a new pointer from a @c std::unique_ptr
		 *
		 * @tparam T The type of the contained object
		 *
		 * @param ptr The pointer to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename T>
		Pointer(std::unique_ptr<T>* ptr, bool isConst) {
			new(mem) StdUniquePtr<T>(ptr, isConst);
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

		/**
		 * @brief Check if the pointer has no stored value
		 *
		 * @return Whether or not the pointer is empty
		 */
		bool isNull() const {
			return impl()->isNull();
		}

		/**
		 * @brief Initialize the pointer with a fresh value
		 */
		void init() {
			impl()->init();
		}

		/**
		 * @brief Access the contents of the pointer as a Var
		 *
		 * @return A Var holding the contents of the pointer, const state inherited
		 */
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