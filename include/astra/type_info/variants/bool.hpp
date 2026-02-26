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
		Bool(bool* value, bool is_const)
		  : _value(value), _is_const(is_const) {
		}

		Expected<None> assign(Var var) {
			auto t = TypeId::get<bool>();
			if(var.type() != t) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::type_name(var.type()),					   //
					reflection::type_name(t)));
			}

			_value = const_cast<bool*>(static_cast<const bool*>(var.raw()));
			_is_const = var.is_const();
			return None();
		}

		void unsafe_assign(void* ptr) {
			_value = static_cast<bool*>(ptr);
			_is_const = false;
		}

		[[nodiscard]] bool get() const {
			return *_value;
		}

		Expected<None> set(bool value) {
			if(_is_const) {
				return Error("Trying to set const value");
			}
			*_value = value;
			return None();
		}

		Var var() {
			return Var(_value, _is_const);
		}

	  private:
		bool* _value;
		bool _is_const;
	};

}