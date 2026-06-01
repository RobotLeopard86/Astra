#pragma once

#include <streambuf>

#include "dll.hpp"

namespace astra {

	class ASTRA_API AppendBuf : public std::streambuf {
	  public:
		explicit AppendBuf(std::string* str)
		  : buf(str) {
		}

		int_type overflow(int_type c) override {
			if(c != EOF) {
				buf->push_back(c);
			}
			return c;
		}

		std::streamsize xsputn(const char* s, std::streamsize n) override {
			buf->append(s, s + n);
			return n;
		}

	  private:
		std::string* buf;
	};

}
