#pragma once

#include "astra/constexpr_map.hpp"
#include "astra/type_query.hpp"
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

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) override {
			auto t = TypeId::get(value);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			value = static_cast<T*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
		}

		/**
		 * @brief Unsafely assign a raw pointer to this object
		 *
		 * @warning This function is for internal use only!
		 *
		 * @param ptr The pointer to assign from
		 */
		void unsafeAssign(void* ptr) override {
			value = static_cast<T*>(ptr);
			isConst = false;
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
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
