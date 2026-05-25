#pragma once

#include <algorithm>
#include <cctype>
#include <string_view>

#include "astra/dll.hpp"
#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "astra/type_id.hpp"
#include "astra/var.hpp"

namespace astra {

	struct ASTRA_API Bool {
		Bool(bool* value, bool isConst)
		  : _value(value), _isConst(isConst) {
		}

		void assign(Var var) {
			auto t = TypeId::get<bool>();
			if(var.type() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.type()),												//
					typeName(t)));
			}

			_value = const_cast<bool*>(static_cast<const bool*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) {
			_value = static_cast<bool*>(ptr);
			_isConst = false;
		}

		[[nodiscard]] bool get() const {
			return *_value;
		}

		void set(bool value) {
			if(_isConst) {
				throw std::runtime_error("Trying to set const value");
			}
			*_value = value;
			return;
		}

		Var var() {
			return Var(_value, _isConst);
		}

	  private:
		bool* _value;
		bool _isConst;
	};

}