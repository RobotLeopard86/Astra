#pragma once

#include <algorithm>
#include <cctype>
#include <string_view>

#include "astra/expected.hpp"
#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "astra/type_id.hpp"
#include "astra/variable/var.hpp"

namespace astra {

	struct Bool {
		Bool(bool* value, bool isConst)
		  : _value(value), _isConst(isConst) {
		}

		Expected<None> assign(Var var) {
			auto t = TypeId::get<bool>();
			if(var.type() != t) {
				return Error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),						   //
					typeName(t)));
			}

			_value = const_cast<bool*>(static_cast<const bool*>(var.raw()));
			_isConst = var.isConst();
			return None();
		}

		void unsafeAssign(void* ptr) {
			_value = static_cast<bool*>(ptr);
			_isConst = false;
		}

		[[nodiscard]] bool get() const {
			return *_value;
		}

		Expected<None> set(bool value) {
			if(_isConst) {
				return Error("Trying to set const value");
			}
			*_value = value;
			return None();
		}

		Var var() {
			return Var(_value, _isConst);
		}

	  private:
		bool* _value;
		bool _isConst;
	};

}