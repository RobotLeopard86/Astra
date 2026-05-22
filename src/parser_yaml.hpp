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

		void deserialize(TypeInfo* info);

	  private:
		void parse(TypeInfo* info);
		void parseStr(TypeInfo* info);
		void parseInd(TypeInfo* info);

		void parseSeq(TypeInfo* info);
		void parseSeq(TypeId nestedType, std::function<void(std::size_t, Var)> add);
		void parseFlowSeq(TypeId nestedType, std::function<void(std::size_t, Var)> add);

		static inline void addToArray(Array& a, std::size_t i, Var var);

		void parseMap(Map& map);
		void parseMap(std::function<void()> add);
		void parseFlowMap(std::function<void()> add);
		inline void parseFlowMap(Map& map);
		void parseMapValue(Map& map);

		inline void addToObj(Object& obj);
		inline void addToMap(Map& map, TypeInfo* infoKey, TypeInfo* infoValue);

		static inline bool isEnd(int token);
		static inline bool isNewLine(int token);
		static inline bool isNull(const std::string& word);

		inline void error(const char* str);
		inline void errorToken(char token);
		inline void errorMatch();

		static inline bool parseBool(std::string& str);
		static inline int64_t parseInt(std::string_view str);
		static inline double parseDouble(std::string_view str);

		inline void next();
		char _token;

		std::unordered_map<std::string, Box> _anchors;
	};

}
