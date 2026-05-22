#pragma once

#include "../isequence.hpp"

namespace astra {

	struct IStack : public ISequence {
		~IStack() override = default;

		using ISequence::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual Var top() = 0;
	};

}
