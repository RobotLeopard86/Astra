#pragma once

#include <istream>

#include "astra/expected.hpp"
#include "astra/type_info/variants/array/array.hpp"
#include "astra/type_info/variants/map/map.hpp"
#include "astra/variable/var.hpp"
#include "lexer_json.yy.h"

namespace astra {

	class TypeInfo;

	class ParserJson : rf_json::LexerJson {
	  public:
		ParserJson(const char* input, std::size_t inputSize);
		explicit ParserJson(std::istream& stream);

		Expected<None> deserialize(TypeInfo* info);

	  private:
		Expected<None> parse(TypeInfo* info, char token);
		Expected<None> parseNext(TypeInfo* info);

		Expected<None> parseArray(TypeId nestedType, std::function<Expected<None>(std::size_t, Var)> add);
		Expected<None> parseObject(TypeInfo* info);
		Expected<None> parseMap(Map& map);

		inline Error error(const char* str);
		inline Error errorToken(char token);
		inline Error errorMatch();

		inline Expected<std::pair<std::string, std::string>> parseTag(std::string_view str);
		static inline bool parseBool(std::string_view str);
		static inline int64_t parseInt(std::string_view str);
		static inline Expected<double> parseDoubleSpecial(std::string_view str);
		static inline double parseDouble(std::string_view str);

		inline void next();
		char _token;
	};

}
