#pragma once

#include "astra/reflection/type_name.hpp"
#include "astra/tools/format.hpp"
#include "iarray.hpp"

namespace astra {

	template<typename T, size_t size_v>
	struct CArray final : public IArray {
		CArray() = delete;

		CArray(T (*array)[size_v], bool isConst)
		  : _array(reinterpret_cast<T*>(array)), _isConst(isConst) {
		}

		Expected<None> assign(Var var) override {
			auto t = TypeId::get(_array);
			if(var.type() != t) {
				return Error(astra::format("Cannot assign type: {} to {}",//
					reflection::typeName(var.type()),					  //
					reflection::typeName(t)));
			}

			_array = static_cast<T*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return None();
		}

		void unsafeAssign(void* ptr) override {
			_array = static_cast<T*>(ptr);
			_isConst = false;
		}

		Var ownVar() const override {
			return Var(_array, TypeId::get<T[size_v]>(), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(_array[i]), nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto i = 0; i < size_v; ++i) {
				callback(Var(&(_array[i]), nestedType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto i = 0; i < size_v; ++i) {
				callback(&(_array[i]));
			}
		}

		size_t size() const override {
			return size_v;
		}

		Expected<Var> front() override {
			return Var(_array, TypeId::get<T>(), _isConst);
		};

		Expected<Var> back() override {
			return Var(&(_array[size_v - 1]), TypeId::get<T>(), _isConst);
		};

		Expected<Var> at(size_t idx) override {
			if(idx >= size_v) {
				return Error(astra::format("Index: {} is out of array's size: {}", idx, size_v));
			}

			return Var(&(_array[idx]), TypeId::get<T>(), _isConst);
		}

		Expected<Var> operator[](size_t idx) override {
			return at(idx);
		}

		Expected<None> fill(Var filler) override {
			auto f = filler.rtCast<T>();

			return f.matchMove(
				[this](T* ptr) -> Expected<None> {//
					for(auto i = 0; i < size_v; i++) {
						_array[i] = *ptr;
					}
					return None();
				},
				[](Error err) -> Expected<None> {//
					return err;
				});
		}

	  private:
		T* _array;
		bool _isConst;
	};

}
