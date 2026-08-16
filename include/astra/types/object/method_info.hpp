#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "astra/dll.hpp"
#include "astra/type_id.hpp"
#include "method_desc.hpp"

namespace astra {
	/**
	 * @brief Representation of a concrete object method
	 */
	class ASTRA_API MethodInfo {
	  public:
		/**
		 * @brief Create a new method info from an object pointer and method description
		 *
		 * @param base Pointer to the object instance (may be @c nullptr for static methods)
		 * @param data The method description
		 */
		MethodInfo(const void* base, const MethodDesc* data)
		  : base(const_cast<void*>(base)), data(data) {
		}

		///@cond
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

		MethodInfo(MethodInfo&& other) {
			if(this == &other) {
				return;
			}
			base = std::exchange(other.base, nullptr);
			data = std::exchange(other.data, nullptr);
		}

		MethodInfo& operator=(MethodInfo&& other) {
			if(this == &other) {
				return *this;
			}
			base = std::exchange(other.base, nullptr);
			data = std::exchange(other.data, nullptr);
			return *this;
		}
		///@endcond

		/**
		 * @brief Invoke the method and ignore the return value if present
		 *
		 * @tparam Args Types of each method argument
		 *
		 * @param args The arguments to pass to the method
		 */
		template<typename... Args>
		void invoke(const Args&... args) const {
			std::vector<Var> vArgs;
			foldArgs(&vArgs, args...);

			return data->invoke(Var(), base, vArgs);
		}

		/**
		 * @brief Invoke the method and capture the return value
		 *
		 * @tparam R Return type of the function
		 * @tparam Args Types of each method argument
		 *
		 * @param args The arguments to pass to the method
		 *
		 * @return The function return value
		 */
		template<typename R, typename... Args>
		R invoke(const Args&... args) const {
			std::vector<Var> vArgs;
			foldArgs(&vArgs, &args...);

			if constexpr(!std::is_void_v<R>) {
				R ret;
				data->invoke(Var(&ret), base, vArgs);
				return ret;
			} else {
				data->invoke(Var(), base, vArgs);
			}
		}

		/**
		 * @brief Check if the method is const or not (can be invoked on a const object)
		 *
		 * @return If the method is const
		 */
		bool isConst() const {
			return data->isConst();
		}

		/**
		 * @brief Check if the method is static or not
		 *
		 * @return If the method is static
		 */
		bool isStatic() const {
			return data->isStatic();
		}

		/**
		 * @brief Check if the method is public or not
		 *
		 * @return If the method is public
		 */
		bool isPublic() const {
			return data->isPublic();
		}

		/**
		 * @brief Check if the method is protected or not
		 *
		 * @return If the method is protected
		 */
		bool isProtected() const {
			return data->isProtected();
		}

		/**
		 * @brief Check if the method is private or not
		 *
		 * @return If the method is private
		 */
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