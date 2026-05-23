#pragma once

#include "../isequence.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API ISet : public ISequence {
		~ISet() override = default;

		using ISequence::forEach;
		virtual void remove(Var value) = 0;
		virtual bool contains(Var value) = 0;
	};

}
