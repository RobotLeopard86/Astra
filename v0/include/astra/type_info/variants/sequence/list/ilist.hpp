#pragma once

#include "../isequence.hpp"

namespace astra {

	struct IList : public ISequence {
		~IList() override = default;

		using ISequence::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual void pushFront(Var value) = 0;
		virtual void popFront() = 0;
		virtual Expected<Var> front() = 0;
		virtual Expected<Var> back() = 0;
	};

}
