#pragma once

#include "../ilist.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API ISet : public IList {
		~ISet() override = default;

		using IList::forEach;
		virtual void remove(Var value) = 0;
		virtual bool contains(Var value) = 0;
	};

}
