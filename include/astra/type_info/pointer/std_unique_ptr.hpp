#pragma once

#include "astra/type_name.hpp"
#include "ipointer.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdUniquePtr : IPointer {
		StdUniquePtr(std::unique_ptr<T>* value, bool isConst)
		  : value(value), isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(value);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.typeId()),												//
					typeName(t)));
			}

			value = static_cast<std::unique_ptr<T>*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			value = static_cast<std::unique_ptr<T>*>(ptr);
			isConst = false;
		}

		Var var() const override {
			return Var(value, isConst);
		}

		bool isNull() const override {
			return value->get() == nullptr;
		}

		void init() override {
			*value = std::make_unique<T>();
		}

		Var getNested() const override {
			if(*value == nullptr) {
				throw std::runtime_error("Pointer is null");
			}
			return Var(value->get(), std::is_const_v<T>);
		}

	  private:
		std::unique_ptr<T>* value;
		bool isConst;
	};

}