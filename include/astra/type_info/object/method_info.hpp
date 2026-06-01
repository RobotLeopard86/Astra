#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "astra/dll.hpp"
#include "astra/type_id.hpp"
#include "method_desc.hpp"

namespace astra {

	class ASTRA_API MethodInfo {
	  public:
		MethodInfo(const void* base, const MethodDesc* data)
		  : base(const_cast<void*>(base)), data(data) {
		}

		MethodInfo(void* base, const MethodDesc* data)
		  : base(base), data(data) {
		}

		MethodInfo(const MethodInfo& other) {
			if(this == &other) {
				return;
			}
			base = other.base;
			data = other.data;
		}

		MethodInfo& operator=(const MethodInfo& other) {
			if(this == &other) {
				return *this;
			}
			base = other.base;
			data = other.data;
			return *this;
		}

		template<typename... Args>
		void invoke(const Args&... args) const {
			std::vector<Var> vArgs;
			foldArgs(&vArgs, args...);

			return data->invoke(Var(), base, vArgs);
		}

		template<typename R, typename... Args>
		R invoke(const Args&... args) const {
			std::vector<Var> vArgs;
			foldArgs(&vArgs, &args...);

			R ret;
			data->invoke(Var(&ret), base, vArgs);
			return ret;
		}

		bool isConst() const {
			return data->isConst();
		}

		bool isStatic() const {
			return data->isStatic();
		}

		bool isPublic() const {
			return data->isPublic();
		}

		bool isProtected() const {
			return data->isProtected();
		}

		bool isPrivate() const {
			return data->isPrivate();
		}

	  private:
		void* base;
		const MethodDesc* data;

		template<typename A, typename... Args>
		void foldArgs(std::vector<Var>* vArgs, const A* arg, const Args*... other) const {
			vArgs->push_back(Var(arg));

			foldArgs(vArgs, other...);
		}

		template<typename A>
		void foldArgs(std::vector<Var>* vArgs, const A* arg) const {

			vArgs->push_back(Var(arg));
		}
	};

}
