#pragma once

#include <type_traits>

#include "access.hpp"
#include "astra/tools/traits.hpp"
#include "astra/type_id.hpp"
#include "astra/variable/var.hpp"

namespace astra {

	struct MethodDesc {
		MethodDesc(Expected<None> (*fn)(Var res, void* obj, const std::vector<Var>& args),//
			Access acc)
		  : _fn(fn), _acc(acc) {
		}

		Expected<None> invoke(Var res, void* obj, const std::vector<Var>& args) const {
			return _fn(res, obj, args);
		}

		bool isConst() const {
			return (_acc & Access::kConst) != Access::kNone;
		}

		bool isStatic() const {
			return (_acc & Access::kStatic) != Access::kNone;
		}

		bool isPublic() const {
			return (_acc & Access::kPublic) != Access::kNone;
		}

		bool isProtected() const {
			return (_acc & Access::kProtected) != Access::kNone;
		}

		bool isPrivate() const {
			return (_acc & Access::kPrivate) != Access::kNone;
		}

		Access access() const {
			return _acc;
		}

	  private:
		Expected<None> (*const _fn)(Var res, void* obj, const std::vector<Var>& args);
		const Access _acc;
	};

}
