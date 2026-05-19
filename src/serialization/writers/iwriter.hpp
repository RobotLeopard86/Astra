#pragma once

#include <cstddef>
#include <cstdint>

namespace astra {

	struct IWriter {
		virtual void write(const void* ptr, std::size_t bytes) = 0;
		virtual void write(uint8_t ch) = 0;
		virtual uint8_t peek() const = 0;
		virtual void stepBack(std::size_t bytes) = 0;
	};

}