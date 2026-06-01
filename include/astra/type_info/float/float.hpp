#pragma once

#include <memory>

#include "floating.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API Float final {
		Float() = delete;

		template<typename T>
		Float(T* value, bool isConst) {
			new(mem) Floating<T>(value, isConst);
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

		std::size_t size() const {
			return impl()->size();
		}

		double get() const {
			return impl()->get();
		}

		void set(double value) {
			return impl()->set(value);
		}

	  private:
		char mem[sizeof(Floating<double>)];

		inline const IFloat* impl() const {
			return reinterpret_cast<const IFloat*>(&mem[0]);
		}

		inline IFloat* impl() {
			return reinterpret_cast<IFloat*>(&mem[0]);
		}
	};

}
