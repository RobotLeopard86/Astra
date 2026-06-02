#pragma once

#include "../ilist.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	class ASTRA_API ISet : public IList {
	  public:
		~ISet() override = default;

		using IList::forEach;
		virtual void remove(Var value) = 0;
		virtual bool contains(Var value) = 0;
	};

}
///@endcond
