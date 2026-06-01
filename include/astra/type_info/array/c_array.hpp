#pragma once

#include "astra/type_name.hpp"
#include "astra/format.hpp"
#include "astra/dll.hpp"
#include "iarray.hpp"

namespace astra {

	template<typename T, std::size_t size_v>
	struct ASTRA_API CArray final : public IArray {
		CArray() = delete;

		CArray(T (*array)[size_v], bool isConst)
		  : array(reinterpret_cast<T*>(array)), isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(array);
			if(var.typeId() != t) {
				throw std::runtime_error(::astra::format("Cannot assign type: {} to {}",//
					typeName(var.typeId()),												//
					typeName(t)));
			}

			array = static_cast<T*>(const_cast<void*>(var.raw()));
			isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			array = static_cast<T*>(ptr);
			isConst = false;
		}

		Var ownVar() const override {
			return Var(array, TypeId::get<T[size_v]>(), isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(array[i]), nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(array[i]), nestedType, isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto i = 0; i < size_v; ++i) {
				callback(&(array[i]));
			}
		}

		std::size_t size() const override {
			return size_v;
		}

		Var front() override {
			return Var(array, TypeId::get<T>(), isConst);
		};

		Var back() override {
			return Var(&(array[size_v - 1]), TypeId::get<T>(), isConst);
		};

		Var at(std::size_t idx) override {
			if(idx >= size_v) {
				throw std::runtime_error(::astra::format("Index: {} is out of array's size: {}", idx, size_v));
			}

			return Var(&(array[idx]), TypeId::get<T>(), isConst);
		}

		Var operator[](std::size_t idx) override {
			return at(idx);
		}

		void fill(Var filler) override {
			auto ptr = filler.rtCast<T>();
			for(auto i = 0; i < size_v; i++) {
				array[i] = *ptr;
			}
		}

	  private:
		T* array;
		bool isConst;
	};

}
