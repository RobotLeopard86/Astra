#pragma once

#include "astra/type_query.hpp"
#include "ipointer.hpp"
#include "astra/dll.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdUniquePtr : IPointer {
		StdUniquePtr(std::unique_ptr<T>* value, bool isConst)
		  : value(value), isConst(isConst) {
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

			value = static_cast<std::unique_ptr<T>*>(const_cast<void*>(var.raw()));
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
			value = static_cast<std::unique_ptr<T>*>(ptr);
			isConst = false;
		}

		/**
		 * @brief Access the contents of this object as a Var
		 *
		 * @return A Var holding the contents of this object, const state inherited
		 */
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