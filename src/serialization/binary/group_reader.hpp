#pragma once

#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "../readers/ireader.hpp"

namespace astra {

	struct GroupReader {
		GroupReader(const GroupReader& other) = delete;

		explicit GroupReader(IReader* reader)//
		  : _reader(reader), _word(0) {
			_reader->read(&_header, 1);
		}

		int64_t readSigneg() const {
			auto neg = isNegative();
			auto value = readOne();

			return static_cast<int64_t>(neg ? -value : value);
		}

		uint64_t readUnsigned() const {
			return readOne();
		}

		double readFloat() const {
			auto uValue = readOne();

			//check the first half for emptyness
			if(reinterpret_cast<uint32_t*>(&uValue)[1] == 0) {
				float fValue;
				std::memcpy(&fValue, &uValue, sizeof(fValue));
				return fValue;
			}

			double value;
			std::memcpy(&value, &uValue, sizeof(value));
			return value;
		}

		std::string readString() const {

			//get size from the reader
			size_t size = 0;
			readData(&size);

			std::string str;
			str.resize(size);

			_reader->read(str.data(), size);

			_reader->read(&_header, 1);
			_word = 0;

			return str;
		}

		bool isNull() const {
			uint32_t v = 0;
			_reader->peek(&v, sizeof(v));
			return v == kNull;
		}

	  private:
		const uint32_t kNull = 0x6E756C6C;//n(6E) u(75) l(6C) l(6C)

		IReader* _reader;

		mutable uint8_t _header;
		mutable unsigned int _word;

		uint64_t readOne() const {
			uint64_t value = 0;
			readData(&value);

			//read next word in the header
			_word++;
			if(_word > 1 && !_reader->empty()) {
				_reader->read(&_header, 1);
				_word = 0;
			}

			return value;
		}

		//read data from reader, do not touch the header
		inline void readData(void* ptr) const {
			auto chunks = _header;
			chunks >>= 4U * (1 - _word);
			chunks &= 0b00000111U;

			_reader->read(ptr, chunks + 1);
		}

		inline bool isNegative() const {
			auto negativeBit = _header;
			negativeBit >>= (4U * (1U - _word) + 3U);
			negativeBit &= 0b00000001U;

			return negativeBit == 1;
		}
	};

}