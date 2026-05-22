#pragma once

#include "../isequence.hpp"

namespace astra {

	struct IQueue : public ISequence {
		~IQueue() override = default;

		using ISequence::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual Var front() = 0;
		virtual Var back() = 0;
	};

}
