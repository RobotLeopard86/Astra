#pragma once

#include "../isequence.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API IList : public ISequence {
		~IList() override = default;

		using ISequence::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual void pushFront(Var value) = 0;
		virtual void popFront() = 0;
		virtual Var front() = 0;
		virtual Var back() = 0;
	};

}
