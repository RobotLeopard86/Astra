#pragma once

#include <array>
#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

#include "iwriter.hpp"

namespace astra {

	struct GroupWriter {
		GroupWriter(const GroupWriter& other) = delete;

		explicit GroupWriter(IWriter* writer)//
		  : _writer(writer), _i(1), _word(0) {
			_group[0] = 0;
		}

		~GroupWriter() {
			if(_i > 1) {//_i == 1 means only header has been written
				_writer->write(&_group[0], _i);
			}
		}

		void writeNull() {
			writeOne(static_cast<uint64_t>(kNull), false);
		}

		void write(bool value) {
			writeOne(static_cast<uint64_t>(value), false);
		}

		void write(std::size_t value) {
			writeOne(value, false);
		}

		void write(std::string_view str) {
			//write string size to the group
			writeOne(str.size(), false);

			//write the group to the stream if it wasn't while writing size
			if(_word != 0) {
				flushHeader();
			}

			//write the string to the stream
			_writer->write(str.data(), str.size());
		}

		void write(const void* ptr, std::size_t size, bool isSigned) {
			uint64_t value = 0;

			if(isSigned) {
				std::memcpy(&value, ptr, size);

				auto alignedZeroes = 64U - size * 8;
				bool neg = ((value << alignedZeroes) & 0x8000000000000000) != 0;

				if(neg) {
					value <<= alignedZeroes;
					value = ~value;
					value >>= alignedZeroes;
					value += 1;
				}

				writeOne(value, neg);
			} else {
				std::memcpy(&value, ptr, size);
				writeOne(value, false);
			}
		}

		void write(double value) {
			uint64_t uValue = 0;

			if(value >= -FLT_MAX && value <= FLT_MAX) {
				float fValue = value;
				std::memcpy(&uValue, &fValue, sizeof(fValue));
			} else {
				std::memcpy(&uValue, &value, sizeof(value));
			}

			writeOne(uValue, false);
		}

	  private:
		const uint32_t kNull = 0x6E756C6C;//n(6E) u(75) l(6C) l(6C)

		IWriter* _writer;
		std::array<uint8_t, 17> _group;
		std::size_t _i;

		unsigned int _word;

		void writeOne(uint64_t value, bool neg) {
			//set sign bit
			_group[0] |= (static_cast<uint8_t>(neg) << 3U);

			//ATTENTION if value == 0 we should write at least one byte
			int volatile zeroes = 63;
			if(value != 0) {
#if __GNUG__
				zeroes = __builtin_clzll(value);
#else
				zeroes = __lzcnt64(value);
#endif
			}

			auto chunks = (64U - zeroes);
			chunks = chunks / 8 - static_cast<uint8_t>((chunks % 8) == 0);

			//set number of bytes
			_group[0] |= chunks;
			//shift if needed
			_group[0] <<= (1U - _word) * 4;

			auto* p = reinterpret_cast<uint8_t*>(&value);
			std::memcpy(&_group[_i], p, chunks + 1);
			_i += chunks + 1;

			//get next word in the header, push one if needed
			_word++;
			if(_word > 1) {
				flushHeader();
			}
		}

		inline void flushHeader() {
			_writer->write(&_group[0], _i);
			_group[0] = 0;
			_i = 1;
			_word = 0;
		}

		inline bool isNegative() {
			auto negativeBit = _group[0];
			negativeBit >>= (4U * (1U - _word) + 3U);
			negativeBit &= 0b00000001U;

			return negativeBit == 1;
		}
	};

}
