#pragma once

#include <istream>


#include "astra/type_info/array/array.hpp"
#include "astra/type_info/map/map.hpp"
#include "astra/var.hpp"
#include "lexer_json.yy.h"

namespace astra {

	class TypeInfo;

	class ParserJson : rf_json::LexerJson {
	  public:
		ParserJson(const char* input, std::size_t inputSize);
		explicit ParserJson(std::istream& stream);

		void deserialize(TypeInfo* info);

	  private:
		void parse(TypeInfo* info, char token);
		void parseNext(TypeInfo* info);

		void parseArray(TypeId nestedType, std::function<void(std::size_t, Var)> add);
		void parseObject(TypeInfo* info);
		void parseMap(Map& map);

		inline void error(const char* str);
		inline void errorToken(char token);
		inline void errorMatch();

		inline std::pair<std::string, std::string> parseTag(std::string_view str);
		static inline bool parseBool(std::string_view str);
		static inline int64_t parseInt(std::string_view str);
		static inline double parseDoubleSpecial(std::string_view str);
		static inline double parseDouble(std::string_view str);

		inline void next();
		char _token;
	};

}
