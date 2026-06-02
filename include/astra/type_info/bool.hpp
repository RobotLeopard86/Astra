#pragma once

#include <cctype>
#include <string_view>

#include "astra/dll.hpp"
#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/type_id.hpp"
#include "astra/var.hpp"

namespace astra {

	struct ASTRA_API Bool {
		Bool(bool* value, bool isConst)
		  : value(value), isConst(isConst) {
		}

		void assign(Var var) {
			auto t = TypeId::get<bool>();
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			value = const_cast<bool*>(static_cast<const bool*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) {
			value = static_cast<bool*>(ptr);
			isConst = false;
		}

		[[nodiscard]] bool get() const {
			return *value;
		}

		void set(bool val) {
			if(isConst) {
				throw std::runtime_error("Trying to set const value");
			}
			*value = val;
			return;
		}

		Var var() {
			return Var(value, isConst);
		}

	  private:
		bool* value;
		bool isConst;
	};

}