#pragma once

#include <vector>

#include "../err_helper.hpp"
#include "astra/dll.hpp"
#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "ivector.hpp"

namespace astra {

	template<typename T>
	struct ASTRA_API StdVector : public IVector, public ListErrHelper {
		StdVector() = delete;

		StdVector(std::vector<T>* vector, bool isConst)
		  : vector(vector),
			isConst(isConst) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) override {
			auto t = TypeId::get(vector);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			vector = static_cast<std::vector<T>*>(const_cast<void*>(var.raw()));
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
			vector = static_cast<std::vector<T>*>(ptr);
		}

		Var ownVar() const override {
			return Var(vector, TypeId::get(vector), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *vector) {
				callback(Var(&entry, nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *vector) {
				callback(Var(&entry, nestedType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *vector) {
				callback(&entry);
			}
		}

		void clear() override {
			vector->clear();
		}

		std::size_t size() const override {
			return vector->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.typeId()) {
				error("Trying to set value to vector<{}>",
					value.typeId(), nestedType);
			}
			vector->push_back(*static_cast<const T*>(value.raw()));
		}

		void pop() override {
			vector->pop_back();
		}

		Var front() override {
			if(vector->empty()) {
				throw std::runtime_error("The vector is empty");
			}
			return Var(&vector->front(), TypeId::get<T>(), isConst);
		};

		Var back() override {
			if(vector->empty()) {
				throw std::runtime_error("The vector is empty");
			}
			return Var(&vector->back(), TypeId::get<T>(), isConst);
		};

		Var at(std::size_t idx) override {
			if(idx >= vector->size()) {
				throw std::runtime_error(::astra::format("Index: {} is out of array's size: {}", idx, vector->size()));
			}

			return Var(&(*vector)[idx], TypeId::get<T>(), isConst);
		}

		Var operator[](std::size_t idx) override {
			return at(idx);
		}

	  private:
		std::vector<T>* vector;
		bool isConst;
	};

}
