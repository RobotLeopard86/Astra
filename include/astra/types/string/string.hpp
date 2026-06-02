#pragma once

#include <cstddef>

#include "c_string.hpp"
#include "astra/sizeof.hpp"
#include "istring.hpp"
#include "std_basic_string.hpp"
#include "std_basic_string_view.hpp"
#include "astra/dll.hpp"

namespace astra {

	class ASTRA_API String final {
	  public:
		String() = delete;

		/**
		 * @brief Create a new string from a @c std::basic_string
		 *
		 * @tparam T The type of characters forming the string (for @c std::string, T = @c char)
		 *
		 * @param str The string to source data from
		 * @param isConst If write operations should be disabled
		 */
		template<typename T>
		String(std::basic_string<T>* str, bool isConst) {
			new(mem) StdBasicString<T>(str, isConst);
		}

		/**
		 * @brief Create a new string from a @c std::basic_string_view
		 *
		 * @tparam T The type of characters forming the string (for @c std::string_view, T = @c char)
		 *
		 * @param view The string view to source data from
		 */
		template<typename T>
		String(std::basic_string_view<T>* view) {
			new(mem) StdBasicStringView<T>(view);
		}

		/**
		 * @brief Create a new string from a raw C string
		 *
		 * @tparam T The type of characters forming the string (will usually be @c char)
		 *
		 * @param strPtr A pointer to the string to source data from
		 */
		template<typename T>
		String(const T** strPtr) {
			new(mem) CString<T>(strPtr);
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a string
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
		 * @brief Get the stored value
		 *
		 * @return The stored value
		 */
		const std::string& get() const {
			return impl()->get();
		}

		/**
		 * @brief Set the stored value
		 *
		 * @param value The new value
		 *
		 * @throws std::runtime_error If write operations are disabled
		 */
		void set(const std::string& value) {
			return impl()->set(value);
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
		Var var() const {
			return impl()->var();
		}

	  private:
		char mem[Sizeof<CString<char>, StdBasicString<char>, StdBasicStringView<char>>::max()];

		inline const IString* impl() const {
			return reinterpret_cast<const IString*>(&mem[0]);
		}

		inline IString* impl() {
			return reinterpret_cast<IString*>(&mem[0]);
		}
	};

}
