#pragma once

#include <memory>

#include "astra/type_info/variants/array/array.hpp"
#include "astra/type_info/variants/map/map.hpp"
#include "astra/type_info/variants/object/object.hpp"
#include "astra/variable/box.hpp"
#include "lexer_yaml.yy.h"

namespace astra {

	class TypeInfo;

	class ParserYaml : rf_yaml::LexerYaml {
	  public:
		ParserYaml(const char* input, std::size_t inputSize);
		explicit ParserYaml(std::istream& stream);

		Expected<None> deserialize(TypeInfo* info);

	  private:
		Expected<None> parse(TypeInfo* info);
		Expected<None> parseStr(TypeInfo* info);
		Expected<None> parseInd(TypeInfo* info);

		Expected<None> parseSeq(TypeInfo* info);
		Expected<None> parseSeq(TypeId nestedType, std::function<Expected<None>(std::size_t, Var)> add);
		Expected<None> parseFlowSeq(TypeId nestedType, std::function<Expected<None>(std::size_t, Var)> add);

		static inline Expected<None> addToArray(Array& a, std::size_t i, Var var);

		Expected<None> parseMap(Map& map);
		Expected<None> parseMap(std::function<Expected<None>()> add);
		Expected<None> parseFlowMap(std::function<Expected<None>()> add);
		inline Expected<None> parseFlowMap(Map& map);
		Expected<None> parseMapValue(Map& map);

		inline Expected<None> addToObj(Object& obj);
		inline Expected<None> addToMap(Map& map, TypeInfo* infoKey, TypeInfo* infoValue);

		static inline bool isEnd(int token);
		static inline bool isNewLine(int token);
		static inline bool isNull(const std::string& word);

		inline Error error(const char* str);
		inline Error errorToken(char token);
		inline Error errorMatch();

		static inline bool parseBool(std::string& str);
		static inline int64_t parseInt(std::string_view str);
		static inline double parseDouble(std::string_view str);

		inline void next();
		char _token;

		std::unordered_map<std::string, Box> _anchors;
	};

}

#undef __retry
