#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "astra/error/error.hpp"
#include "astra/type_id.hpp"
#include "method_desc.hpp"

namespace astra {

	class MethodInfo {
	  public:
		MethodInfo(const void* base, const MethodDesc* data)
		  : _base(const_cast<void*>(base)), _data(data) {
		}

		MethodInfo(void* base, const MethodDesc* data)
		  : _base(base), _data(data) {
		}

		MethodInfo(const MethodInfo& other) {
			if(this == &other) {
				return;
			}
			_base = other._base;
			_data = other._data;
		}

		MethodInfo& operator=(const MethodInfo& other) {
			if(this == &other) {
				return *this;
			}
			_base = other._base;
			_data = other._data;
			return *this;
		}

		template<typename... Args>
		void invoke(const Args&... args) const {
			std::vector<Var> vArgs;
			foldArgs(&vArgs, args...);

			return _data->invoke(Var(), _base, vArgs);
		}

		template<typename RetT, typename... Args>
		RetT invoke(const Args&... args) const {
			std::vector<Var> vArgs;
			foldArgs(&vArgs, &args...);

			RetT ret;
			_data->invoke(Var(&ret), _base, vArgs);
			return ret;
		}

		bool isConst() const {
			return _data->isConst();
		}

		bool isStatic() const {
			return _data->isStatic();
		}

		bool isPublic() const {
			return _data->isPublic();
		}

		bool isProtected() const {
			return _data->isProtected();
		}

		bool isPrivate() const {
			return _data->isPrivate();
		}

	  private:
		void* _base;
		const MethodDesc* _data;

		template<typename ArgT, typename... Args>
		void foldArgs(std::vector<Var>* vArgs, const ArgT* arg, const Args*... other) const {
			vArgs->push_back(Var(arg));

			foldArgs(vArgs, other...);
		}

		template<typename ArgT>
		void foldArgs(std::vector<Var>* vArgs, const ArgT* arg) const {

			vArgs->push_back(Var(arg));
		}
	};

}
