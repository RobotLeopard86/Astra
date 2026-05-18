#pragma once

#include "../isequence.hpp"

namespace astra {

	struct ISet : public ISequence {
		~ISet() override = default;

		using ISequence::forEach;
		virtual Expected<None> remove(Var value) = 0;
		virtual bool contains(Var value) = 0;
	};

}
