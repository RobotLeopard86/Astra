#pragma once

#include "../ilist.hpp"
#include "astra/dll.hpp"

///@cond
namespace astra {

	class ASTRA_API IDoubleEndedList : public IList {
	  public:
		~IDoubleEndedList() override = default;

		using IList::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual void pushFront(Var value) = 0;
		virtual void popFront() = 0;
		virtual Var front() = 0;
		virtual Var back() = 0;
	};

}
///@endcond
