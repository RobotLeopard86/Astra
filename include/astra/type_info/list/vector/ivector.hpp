#pragma once

#include "../ilist.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API IVector : public IList {
		~IVector() override = default;

		using IList::forEach;
		virtual void forEach(std::function<void(Var)> callback) = 0;
		virtual void pop() = 0;
		virtual Var front() = 0;
		virtual Var back() = 0;
		virtual Var at(std::size_t idx) = 0;
		virtual Var operator[](std::size_t idx) = 0;
	};

}
