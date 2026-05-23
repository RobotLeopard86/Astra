#pragma once

#include <memory>

#include "float.hpp"
#include "ifloating.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Floating final {
		Floating() = delete;

		template<typename T>
		Floating(T* value, bool isConst) {
			new(_mem) Float<T>(value, isConst);
		}

		void assign(Var var) {
			return impl()->assign(var);
		}

		void unsafeAssign(void* ptr) {
			return impl()->unsafeAssign(ptr);
		}

		Var var() {
			return impl()->var();
		}

		std::size_t size() {
			return impl()->size();
		}

		double get() const {
			return impl()->get();
		}

		void set(double value) {
			return impl()->set(value);
		}

	  private:
		char _mem[sizeof(Float<double>)];

		inline const IFloating* impl() const {
			return reinterpret_cast<const IFloating*>(&_mem[0]);
		}

		inline IFloating* impl() {
			return reinterpret_cast<IFloating*>(&_mem[0]);
		}
	};

}
