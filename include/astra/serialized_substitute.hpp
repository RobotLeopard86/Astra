#pragma once

namespace astra {
	/**
	 * @brief Template for specifying a substitute reflectable type for a non-reflectable type T.
	 *
	 * The default specialization is marked as invalid. To provide a substitute, specialize this template for your type T.
	 */
	template<typename T>
	struct SerializedSubstitute {
		static constexpr bool invalid = true;
	};
}
