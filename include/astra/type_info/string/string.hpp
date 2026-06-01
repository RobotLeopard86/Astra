#pragma once

#include <cstddef>

#include "c_string.hpp"
#include "astra/sizeof.hpp"
#include "istring.hpp"
#include "std_basic_string.hpp"
#include "std_basic_string_view.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API String final {
		String() = delete;

		template<typename T>
		String(std::basic_string<T>* str, bool isConst) {
			new(mem) StdBasicString<T>(str, isConst);
		}

		template<typename T>
		String(std::basic_string_view<T>* str, bool isConst) {
			new(mem) StdBasicStringView<T>(str);
		}

		template<typename T>
		String(const T** str, bool isConst) {
			new(mem) CString<T>(str);
		}

		void assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		const std::string& get() const {
			return impl()->get();
		}

		void set(const std::string& value) {
			return impl()->set(value);
		}

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
