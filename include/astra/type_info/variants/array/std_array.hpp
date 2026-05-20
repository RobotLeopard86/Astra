#pragma once

#include <vector>

#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "iarray.hpp"

namespace astra {

	template<typename T, std::size_t size_v>
	struct StdArray final : public IArray {
		StdArray() = delete;

		StdArray(std::array<T, size_v>* array, bool isConst)
		  : _array(array), _isConst(isConst) {
		}

		Expected<None> assign(Var var) override {
			auto t = TypeId::get(_array);
			if(var.type() != t) {
				return Error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),						   //
					typeName(t)));
			}

			_array = static_cast<std::array<T, size_v>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_array = static_cast<std::array<T, size_v>*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_array, TypeId::get(_array), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(*_array)[i], nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(*_array)[i]));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto i = 0; i < size_v; ++i) {
				callback(&(*_array)[i]);
			}
		}

		std::size_t size() const override {
			return size_v;
		}

		Expected<Var> front() override {
			return Var(&(*_array)[0], TypeId::get<T>(), _isConst);
		};

		Expected<Var> back() override {
			return Var(&(*_array)[size_v - 1], TypeId::get<T>(), _isConst);
		};

		Expected<Var> at(std::size_t idx) override {
			if(idx >= size_v) {
				return Error(format("Index: {} is out of array's size: {}", idx, size_v));
			}

			return Var(&(*_array)[idx], TypeId::get<T>(), _isConst);
		}

		Expected<Var> operator[](std::size_t idx) override {
			return at(idx);
		}

		Expected<None> fill(Var filler) override {
			auto f = filler.rtCast<T>();

			return f.matchMove(
				[this](T* ptr) -> Expected<None> {//
					_array->fill(*ptr);
					return None();
				},
				[](Error&& err) -> Expected<None> {//
					return err;
				});
		}

	  private:
		std::array<T, size_v>* _array;
		bool _isConst;
	};

}
