#pragma once

#include <type_traits>

#include "access.hpp"
#include "astra/traits.hpp"
#include "astra/type_id.hpp"
#include "astra/var.hpp"
#include "astra/dll.hpp"

namespace astra {

	class ASTRA_API MethodDesc {
	  public:
		/**
		 * @brief Create a new method description
		 *
		 * @param fn A function to invoke, taking in a Var to output the result to, an object to operate on, and the function's arguments as Vars
		 * @param acc The method's access rules
		 */
		MethodDesc(void (*fn)(Var res, void* obj, const std::vector<Var>& args), Access acc)
		  : fn(fn), acc(acc) {}

		/**
		 * @brief Invoke the stored method on a given object
		 *
		 * @param res A Var to output the method result to
		 * @param obj The object to call the method on
		 * @param args The arguments to the method call, as Vars
		 */
		void invoke(Var res, void* obj, const std::vector<Var>& args) const {
			return fn(res, obj, args);
		}

		/**
		 * @brief Get the method's access bitmask
		 *
		 * @return The method's access
		 */
		Access access() const {
			return acc;
		}

		/**
		 * @brief Check if the method is const or not (can be invoked on a const object)
		 *
		 * @return If the method is const
		 */
		bool isConst() const {
			return (acc & Access::Const) != Access::None;
		}

		/**
		 * @brief Check if the method is static or not
		 *
		 * @return If the method is static
		 */
		bool isStatic() const {
			return (acc & Access::Static) != Access::None;
		}

		/**
		 * @brief Check if the method is public or not
		 *
		 * @return If the method is public
		 */
		bool isPublic() const {
			return (acc & Access::Public) != Access::None;
		}

		/**
		 * @brief Check if the method is protected or not
		 *
		 * @return If the method is protected
		 */
		bool isProtected() const {
			return (acc & Access::Protected) != Access::None;
		}

		/**
		 * @brief Check if the method is private or not
		 *
		 * @return If the method is private
		 */
		bool isPrivate() const {
			return (acc & Access::Private) != Access::None;
		}

	  private:
		void (*const fn)(Var res, void* obj, const std::vector<Var>& args);
		const Access acc;
	};

}