#pragma once

#include <vector>

#include "../err_helper.hpp"
#include "astra/reflection/type_name.hpp"
#include "ivector.hpp"

namespace astra {

	template<typename T>
	struct StdVector : public IVector, public sequence::ErrHelper {
		StdVector() = delete;

		StdVector(std::vector<T>* vector, bool isConst)
		  : _vector(vector),//
			_isConst(isConst) {
		}

		void assign(Var var) override {
			auto t = TypeId::get(_vector);
			if(var.type() != t) {
				throw std::runtime_error(format("Cannot assign type: {} to {}",//
					typeName(var.type()),									   //
					typeName(t)));
			}

			_vector = static_cast<std::vector<T>*>(const_cast<void*>(var.raw()));
			_isConst = var.isConst();
			return;
		}

		void unsafeAssign(void* ptr) override {
			_vector = static_cast<std::vector<T>*>(ptr);
		}

		Var ownVar() const override {
			return Var(_vector, TypeId::get(_vector), _isConst);
		}

		TypeId nestedType() const override {
			return TypeId::get<T>();
		}

		void forEach(std::function<void(Var)> callback) const override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *_vector) {
				callback(Var(&entry, nestedType, true));
			}
		}

		void forEach(std::function<void(Var)> callback) override {
			const auto nestedType = TypeId::get<T>();

			for(auto&& entry : *_vector) {
				callback(Var(&entry, nestedType, _isConst));
			}
		}

		void unsafeForEach(std::function<void(void*)> callback) const override {
			for(auto&& entry : *_vector) {
				callback(&entry);
			}
		}

		void clear() override {
			_vector->clear();
		}

		std::size_t size() const override {
			return _vector->size();
		}

		void push(Var value) override {
			auto nestedType = TypeId::get<T>();

			if(nestedType != value.type()) {
				error("Trying to set value to vector<{}>",//
					value.type(), nestedType);
			}
			_vector->push_back(*static_cast<const T*>(value.raw()));
			return;
		}

		void pop() override {
			_vector->pop_back();
		}

		Var front() override {
			if(_vector->empty()) {
				throw std::runtime_error("The vector is empty");
			}
			return Var(&_vector->front(), TypeId::get<T>(), _isConst);
		};

		Var back() override {
			if(_vector->empty()) {
				throw std::runtime_error("The vector is empty");
			}
			return Var(&_vector->back(), TypeId::get<T>(), _isConst);
		};

		Var at(std::size_t idx) override {
			if(idx >= _vector->size()) {
				throw std::runtime_error(format("Index: {} is out of array's size: {}", idx, _vector->size()));
			}

			return Var(&(*_vector)[idx], TypeId::get<T>(), _isConst);
		}

		Var operator[](std::size_t idx) override {
			return at(idx);
		}

	  private:
		std::vector<T>* _vector;
		bool _isConst;
	};

}
