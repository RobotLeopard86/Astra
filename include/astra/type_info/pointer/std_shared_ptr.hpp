#pragma once

#include "astra/type_name.hpp"
#include "ipointer.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdSharedPtr : IPointer {
		StdSharedPtr(std::shared_ptr<T>* value, bool isConst)
		  : _value(value), _isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_value);
			if(var.type() != t) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(t)));
			}

			_value = static_cast<std::shared_ptr<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_value = static_cast<std::shared_ptr<T>*>(ptr);
			_isConst = false;
		}

		Var var() const override {
			return Var(_value, _isConst);
		}

		bool isNull() const override {
			return _value->get() == nullptr;
		}

		void init() override {
			*_value = std::make_shared<T>();
		}

		Var getNested() const override {
			if(*_value == nullptr) {
				throw std::runtime_error("Pointer is null");
			}
			return Var(_value->get(), std::is_const_v<T>);
		}

	  private:
		std::shared_ptr<T>* _value;
		bool _isConst;
	};

}