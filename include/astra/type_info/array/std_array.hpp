#pragma once

#include <vector>

#include "astra/type_query.hpp"
#include "astra/format.hpp"
#include "astra/dll.hpp"
#include "iarray.hpp"

namespace astra {

	template<typename T, std::size_t size_v>
	struct ASTRA_API StdArray final : public IArray {
		StdArray() = delete;

		StdArray(std::array<T, size_v>* array, bool isConst)
		  : array(array), isConst(isConst) {
		}

		/**
		 * @brief Assign the contents of a Var to this object
		 *
		 * @param var The Var to assign from
		 *
		 * @throws std::runtime_error If the source Var does not contain a bool
		 */
		void assign(Var var) override {
			auto t = TypeId::get(array);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",
					typeName(var.typeId()),
					typeName(t)));
			}

			array = static_cast<std::array<T, size_v>*>(const_cast<void*>(var.raw()));
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
			array = static_cast<std::array<T, size_v>*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(array, TypeId::get(array), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(*array)[i], nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(*array)[i]));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto i = 0; i < size_v; ++i) {
				callback(&(*array)[i]);
			}
		}

		std::size_t size() const override {
			return size_v;
		}

		Var front() override {
			return Var(&(*array)[0], TypeId::get<T>(), isConst);
		};

		Var back() override {
			return Var(&(*array)[size_v - 1], TypeId::get<T>(), isConst);
		};

		Var at(std::size_t idx) override {
			if(idx >= size_v) {
				throw std::runtime_error(::astra::format("Index: {} is out of array's size: {}", idx, size_v));
			}

			return Var(&(*array)[idx], TypeId::get<T>(), isConst);
		}

		Var operator[](std::size_t idx) override {
			return at(idx);
		}

		void fill(Var filler) override {
			auto ptr = filler.rtCast<T>();
			array->fill(*ptr);
		}

	  private:
		std::array<T, size_v>* array;
		bool isConst;
	};

}
