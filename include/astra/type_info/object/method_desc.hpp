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
		  : _fn(fn), _acc(acc) {
		}

		void invoke(Var res, void* obj, const std::vector<Var>& args) const {
			return _fn(res, obj, args);
		}

		bool isConst() const {
			return (_acc & Access::Const) != Access::None;
		}

		bool isStatic() const {
			return (_acc & Access::Static) != Access::None;
		}

		bool isPublic() const {
			return (_acc & Access::Public) != Access::None;
		}

		bool isProtected() const {
			return (_acc & Access::Protected) != Access::None;
		}

		bool isPrivate() const {
			return (_acc & Access::Private) != Access::None;
		}

		Access access() const {
			return _acc;
		}

	  private:
		void (*const _fn)(Var res, void* obj, const std::vector<Var>& args);
		const Access _acc;
	};

}
