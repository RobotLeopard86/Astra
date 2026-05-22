#pragma once

#include "astra/constexpr_map.hpp"
#include "astra/reflection/type_name.hpp"
#include "ienum.hpp"

#include <cstdint>

namespace astra {

	template<typename T, std::size_t size_v>
	struct EnumImpl final : public IEnum {
		EnumImpl() = delete;

		EnumImpl(T* value, bool isConst, const ConstexprMap<T, size_v>& allConstants)
		  : _value(value), _isConst(isConst), _all_constants(allConstants) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_value);
			if(var.type() != t) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(t)));
			}

			_value = static_cast<T*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_value = static_cast<T*>(ptr);
			_isConst = false;
		}

		Var var() const override {
			return Var(_value, TypeId::get<T>(), _isConst);
		}

		std::string_view toString() const override {
			return _all_constants.getName(*_value);
		}

		void fromString(std::string_view name) override {
			if(_isConst) {
				throw std::runtime_error("Cannot assign anything to const enum");
			}
			auto v = _all_constants.getValue(name);
			*_value = v;
		}

	  private:
		T* _value;
		bool _isConst;
		const ConstexprMap<T, size_v>& _all_constants;
	};

}
