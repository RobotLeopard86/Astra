#pragma once

#include <memory>

#include "enum_impl.hpp"
#include "ienum.hpp"

namespace astra {

	///container of all field names matched to Var structs
	///just a registry of types and value pointers
	struct Enum final {

		template<typename T, std::size_t size>
		Enum(T* value, bool isConst, const ConstexprMap<T, size>& allConstants) {
			new(_mem) EnumImpl<T, size>(value, isConst, allConstants);
		}

		Expected<None> assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			impl()->unsafeAssign(ptr);
		}

		Var var() const {
			return impl()->var();
		}

		std::string_view toString() const {
			return impl()->toString();
		}

		Expected<None> fromString(std::string_view name) {
			return impl()->fromString(name);
		};

	  private:
		char _mem[sizeof(EnumImpl<int, 0>)];

		inline const IEnum* impl() const {
			return reinterpret_cast<const IEnum*>(&_mem[0]);
		}

		inline IEnum* impl() {
			return reinterpret_cast<IEnum*>(&_mem[0]);
		}
	};

}
