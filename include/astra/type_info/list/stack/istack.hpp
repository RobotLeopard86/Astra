#pragma once

#include "../ilist.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API IStack : public IList {
		~IStack() override = default;

		using IList::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual Var top() = 0;
	};

}
