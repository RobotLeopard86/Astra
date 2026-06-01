#pragma once

#include <type_traits>

#include "access.hpp"
#include "astra/traits.hpp"
#include "astra/type_id.hpp"
#include "astra/var.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API MethodDesc {
		MethodDesc(void (*fn)(Var res, void* obj, const std::vector<Var>& args),//
			Access acc)
		  : fn(fn), acc(acc) {
		}

		void invoke(Var res, void* obj, const std::vector<Var>& args) const {
			return fn(res, obj, args);
		}

		bool isConst() const {
			return (acc & Access::Const) != Access::None;
		}

		bool isStatic() const {
			return (acc & Access::Static) != Access::None;
		}

		bool isPublic() const {
			return (acc & Access::Public) != Access::None;
		}

		bool isProtected() const {
			return (acc & Access::Protected) != Access::None;
		}

		bool isPrivate() const {
			return (acc & Access::Private) != Access::None;
		}

		Access access() const {
			return acc;
		}

	  private:
		void (*const fn)(Var res, void* obj, const std::vector<Var>& args);
		const Access acc;
	};

}
