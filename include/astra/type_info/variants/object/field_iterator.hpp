#pragma once

#include <cstddef>
#include <map>
#include <string_view>

#include "field_info.hpp"

namespace astra {

	///Skip iterator for fields collection, steps over items with unwanted properties
	class FieldIterator {
		using const_iterator = std::map<std::string_view, FieldDesc>::const_iterator;

	  public:
		FieldIterator(const std::map<std::string_view, FieldDesc>* map,//
			const void* base,										   //
			Access acc,												   //
			bool includeReadonly)
		  : _it(map->begin()),//
			_end(map->cend()),//
			_base(base),	  //
			_acc(acc),		  //
			_include_readonly(includeReadonly) {
			//start from a valid element
			if(!isValid()) {
				nextValid();
			}
		}

		FieldIterator& operator++() noexcept {
			nextValid();
			return *this;
		};

		FieldIterator operator++(int) noexcept {
			auto t = *this;
			++(*this);
			return t;
		};

		bool operator==(const const_iterator& other) const noexcept {
			return _it != other;
		};

		bool operator!=(const const_iterator& other) const noexcept {
			return _it != other;
		};

		auto operator*() const noexcept {
			return std::make_pair(_it->first, FieldInfo(_base, &_it->second));
		};

	  private:
		const_iterator _it;
		const const_iterator _end;
		const void* _base;
		const Access _acc;
		const bool _include_readonly;

		bool isValid() {
			return rightAccess() &&		   //
				   rightStaticReadonly() &&//
				   (_base != nullptr || _it->second.isStatic());
		}

		void nextValid() {
			do {
				++_it;
			} while(_it != _end && !isValid());
		}

		inline bool rightAccess() {
			return (_it->second.access() & _acc & (Access::kPublic | Access::kProtected | Access::kPrivate)) != Access::kNone;
		}

		//built from Karnaugh Map
		inline bool rightStaticReadonly() {
			bool nx0 = !_it->second.isReadonly();
			bool nx1 = (_it->second.access() & Access::kStatic) == Access::kNone;
			bool x2 = _include_readonly;
			bool x3 = (_acc & Access::kStatic) != Access::kNone;

			return (nx0 || x2) && (nx1 || x3);
		}
	};

}
