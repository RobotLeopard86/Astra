#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

#include "iinteger.hpp"
#include "int.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Integer {
		Integer() = delete;

		template<typename T>
		Integer(T* value, bool isConst) {
			new(_mem) Int<T>(value, isConst);
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

		std::size_t size() const {
			return impl()->size();
		}

		bool isSigned() const {
			return impl()->isSigned();
		}

		int64_t asSigned() const {
			return impl()->asSigned();
		}

		uint64_t asUnsigned() const {
			return impl()->asUnsigned();
		}

		template<typename T>
		typename std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>,//
			void>
		set(T value) {
			return impl()->setSigned(value);
		}

		template<typename T>
		typename std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>,//
			void>
		set(T value) {
			return impl()->setUnsigned(value);
		}

		void setSigned(int64_t value) {
			return impl()->setSigned(value);
		}

		void setUnsigned(uint64_t value) {
			return impl()->setUnsigned(value);
		}

	  private:
		char _mem[sizeof(Int<int>)];

		inline const IInteger* impl() const {
			return reinterpret_cast<const IInteger*>(&_mem[0]);
		}

		inline IInteger* impl() {
			return reinterpret_cast<IInteger*>(&_mem[0]);
		}
	};

}
