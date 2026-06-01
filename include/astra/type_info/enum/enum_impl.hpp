#pragma once

#include "astra/constexpr_map.hpp"
#include "astra/type_name.hpp"
#include "astra/dll.hpp"
#include "ienum.hpp"

#include <cstdint>

namespace astra {

	template<typename T, std::size_t size_v>
	struct ASTRA_API EnumImpl final : public IEnum {
		EnumImpl() = delete;

		EnumImpl(T* value, bool isConst, const ConstexprMap<T, size_v>& allConstants)
		  : value(value), isConst(isConst), all_constants(allConstants) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(value);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			value = static_cast<T*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			value = static_cast<T*>(ptr);
			isConst = false;
		}

		Var var() const override {
			return Var(value, TypeId::get<T>(), isConst);
		}

		const std::string& toString() const override {
			return all_constants.getKey(*value);
		}

		void fromString(const std::string& name) override {
			if(isConst) {
				throw std::runtime_error("Cannot assign anything to const enum");
			}
			auto v = all_constants.getValue(name);
			*value = v;
		}

	  private:
		T* value;
		bool isConst;
		const ConstexprMap<T, size_v>& all_constants;
	};

}
