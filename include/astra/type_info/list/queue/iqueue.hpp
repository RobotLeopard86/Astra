#pragma once

#include "../ilist.hpp"

#include "astra/dll.hpp"

///@cond
namespace astra {

	class ASTRA_API IQueue : public IList {
	  public:
		~IQueue() override = default;

		using IList::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual Var front() = 0;
		virtual Var back() = 0;
	};

}
///@endcond
