#pragma once

#include <cstdint>

namespace astra {

	/**
	 * @brief Flags for access modifiers on object members (works as bitmask)
	 */
	enum class Access : uint8_t {
		None = 0b00000,
		Public = 0b00001,
		Protected = 0b00010,
		Private = 0b00100,
		Static = 0b01000,
		Const = 0b10000,
		All = 0b11111,
	};

	///@cond
	inline Access operator|(Access lhs, Access rhs) {
		return static_cast<Access>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
	}

	inline Access operator&(Access lhs, Access rhs) {
		return static_cast<Access>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}
	///@endcond

}
