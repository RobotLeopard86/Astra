#pragma once

#include <cstddef>

#include "c_string.hpp"
#include "astra/tools/sizeof.hpp"
#include "istring.hpp"
#include "std_basic_string.hpp"
#include "std_basic_string_view.hpp"

namespace astra {

	struct String final {
		String() = delete;

		template<typename T>
		String(std::basic_string<T>* str, bool isConst) {
			new(_mem) StdBasicString<T>(str, isConst);
		}

		template<typename T>
		String(std::basic_string_view<T>* str, bool isConst) {
			new(_mem) StdBasicStringView<T>(str);
		}

		template<typename T>
		String(const T** str, bool isConst) {
			new(_mem) CString<T>(str);
		}

		void assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		std::string_view get() const {
			return impl()->get();
		}

		void set(std::string_view value) {
			return impl()->set(value);
		}

		Var var() const {
			return impl()->var();
		}

	  private:
		char _mem[Sizeof<CString<char>, StdBasicString<char>, StdBasicStringView<char>>::max()];

		inline const IString* impl() const {
			return reinterpret_cast<const IString*>(&_mem[0]);
		}

		inline IString* impl() {
			return reinterpret_cast<IString*>(&_mem[0]);
		}
	};

}
