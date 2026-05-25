#include "parser_json.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>


#include "astra/reflection.hpp"
#include "astra/types/all_types.hpp"
#include "astra/box.hpp"

namespace astra {

	ParserJson::ParserJson(const char* input, std::size_t inputSize)//
	  : LexerJson(input, inputSize) {
	}

	ParserJson::ParserJson(std::istream& stream)//
	  : LexerJson(stream) {
	}

	void ParserJson::deserialize(TypeInfo* info) {
		return parseNext(info);
	}

	void ParserJson::parse(TypeInfo* info, char token) {
		if(getWord() == "null") {
			//do nothing, just skip the field
			return;
		}

		if(info->is<Pointer>()) {
			auto p = info->asUnsafe<Pointer>();
			try {
				Var var = p.getNested();
				TypeInfo nestedInfo = reflect(var);
				return parse(&nestedInfo, token);
			} catch(...) {
				p.init();
				TypeInfo nestedInfo = reflect(p.var());
				return parse(&nestedInfo, token);
			}
		}

		switch(token) {
				// clang-format off
    case 's': {
      return info->match(
          [this](Bool& b) -> void {  //
            return b.set(parseBool(getWord()));
          },
          [this](Integer& i) -> void {
            auto w = getWord();

            if (w.front() == '-' || i.isSigned()) {
              return i.setSigned(std::strtoll(w.data(), nullptr, 10));
            }
            return i.setUnsigned(std::strtoull(w.data(), nullptr, 10));
          },
          [this](Floating& f) -> void {  //
            return f.set(parseDouble(getWord()));
          },
          [this](auto&&) -> void {  //
           	errorMatch();
          });
    }
    case '$':
      return info->match(
          [this](String& s) -> void {
            s.set(getWord());
          },
          [this](Enum& e) -> void {
            e.fromString(getWord());
          },
          [this](Floating& f) -> void {
            f.set(parseDoubleSpecial(getWord()));
          },
          [this](auto&&) -> void {
            errorMatch();
          });
    case '[':
      return info->match(
          [this](Array& a) -> void {
            return parseArray(a.nestedType(), [&](std::size_t i, Var var) -> void {
              if (i < a.size()) {
                auto item = a.at(i);
                return copy(item, var);
              }
              return;
            });
          },
          [this](Sequence& s) -> void {
            s.clear();
            return parseArray(s.nestedType(), [&](std::size_t, Var var) {
              return s.push(var);
            });
          },
          [this](Map& m) -> void {
            return parseMap(m);
          },
          [this](auto&&) -> void {
            errorMatch();
          });
			// clang-format on
			case '{':
				return parseObject(info);
			default:
				errorToken(token);
		}
	}

	void ParserJson::parseNext(TypeInfo* info) {
		next();
		return parse(info, _token);
	}

	void ParserJson::parseArray(TypeId nestedType, std::function<void(std::size_t, Var)> add) {
		next();//skip '['
		if(_token == ']') {
			//an empty array
			return;
		}

		//save few ns for each iteration of the loop
		auto boxedInfo = reflect(Var(nullptr, nestedType, false));

		for(std::size_t i = 0; /**/; ++i) {
			Box box(nestedType);//Box should be a new object for each iteration
			boxedInfo.unsafeAssign(box.var().rawMut());

			parse(&boxedInfo, _token);
			add(i, box.var());

			next();
			if(_token == ']') {
				return;
			}
			if(_token != ',') {
				errorToken(_token);
			}

			//get another one
			next();
		}

		return;
	}

	void ParserJson::parseObject(TypeInfo* info) {
		next();//skip '{'
		if(_token == '}') {
			//an empty object
			return;
		}

		auto o = info->as<Object>();

		while(true) {
			if(_token != '$') {
				error("Cannot reach a field name");
			}
			next();
			if(_token != ':') {
				error("Cannot reach a field value");
			}

			auto field = o.getField(getWord()).var();
			auto fieldInfo = reflect(field);
			parseNext(&fieldInfo);

			next();
			if(_token == '}') {
				return;
			}
			if(_token != ',') {
				errorToken(_token);
			}

			next();
		}

		error("Max depth level exceeded");
	}

	void ParserJson::parseMap(Map& map) {
		map.clear();

		next();//skip '['
		if(_token == ']') {
			//an empty map
			return;
		}

		std::string key = "key";
		std::string val = "val";

		if(_token == '$') {
			//if particular tag found parse it
			auto pos = getWord().find("!!map");
			if(pos != std::string::npos) {
				auto pair = parseTag(getWord());
				key = std::move(pair.first);
				val = std::move(pair.second);

				//make step to get a new token
				next();
				if(_token != ',') {
					errorToken(_token);
				}
				next();
			} else {
				error("Cannot reach the map tag or '{'");
			}
		}

		else if(_token != '{') {
			errorToken(_token);
		}

		auto keyInfo = reflect(Var(nullptr, map.keyType(), false));
		auto valInfo = reflect(Var(nullptr, map.valType(), false));

		//token '{' has already been read
		while(true) {
			Box keyBox(map.keyType());//Box should be a new object for each iteration
			keyInfo.unsafeAssign(keyBox.var().rawMut());

			Box valBox(map.valType());
			valInfo.unsafeAssign(valBox.var().rawMut());

			//parse first field key or val
			next();
			if(_token != '$') {
				error("Cannot reach a field name");
			}

			next();
			if(_token != ':') {
				error("Cannot reach a field value");
			}

			if(getWord() == key) {
				parseNext(&keyInfo);
			} else if(getWord() == val) {
				parseNext(&valInfo);
			} else {
				throw std::runtime_error(::astra::format("Got an unexpected field '{}' while parse map; {}",//
					getWord(), getPosition().toString()));
			}

			next();
			if(_token == '}') {
				error("Unexpected end of JSON object");
			}
			if(_token != ',') {
				errorToken(_token);
			}

			//parse second field key or val
			next();
			if(_token != '$') {
				error("Cannot reach a field name");
			}

			next();
			if(_token != ':') {
				error("Cannot reach a field value");
			}

			if(getWord() == key) {
				parseNext(&keyInfo);
			} else if(getWord() == val) {
				parseNext(&valInfo);
			} else {
				throw std::runtime_error(::astra::format("Got an unexpected field '{}' while parse map; {}",//
					getWord(), getPosition().toString()));
			}

			map.insert(keyBox.var(), valBox.var());

			next();
			if(_token == '}') {
				next();
			}
			if(_token == ']') {
				return;
			}
			if(_token != ',') {
				errorToken(_token);
			}

			//take next '{'
			next();
		}
	}

	void ParserJson::next() {
		_token = static_cast<char>(lex());
	}

	void ParserJson::error(const char* str) {
		throw std::runtime_error(::astra::format("{}; {}", str, getPosition().toString()));
	}

	void ParserJson::errorToken(char token) {
		throw std::runtime_error(::astra::format("Unexpected token '{}'; {}", token, getPosition().toString()));
	}

	void ParserJson::errorMatch() {
		throw std::runtime_error(::astra::format("Cannot match correct type; {}", getPosition().toString()));
	}

	std::pair<std::string, std::string> ParserJson::parseTag(std::string_view str) {
		auto pos1 = str.find('|');
		if(pos1 == std::string::npos) {
			error("Cannot find '|' in the tag");
		}
		auto pos2 = str.find(':');
		if(pos2 == std::string::npos) {
			error("Cannot find ':' in the tag");
		}

		auto key = std::string(str.substr(pos1 + 1, pos2 - (pos1 + 1)));
		auto val = std::string(str.substr(pos2 + 1, str.size() - (pos2 + 1)));

		return std::make_pair(std::move(key), std::move(val));
	}

	bool ParserJson::parseBool(std::string_view str) {
		return str != "false";
	}

	double ParserJson::parseDoubleSpecial(std::string_view str) {
		if(str == "-inf") {
			return -std::numeric_limits<double>::infinity();
		}
		if(str == "inf") {
			return std::numeric_limits<double>::infinity();
		}
		if(str == "nan") {
			return std::nan("");
		}
		throw std::runtime_error(::astra::format("Expected -inf, inf, nan but {} reached", str));
	}

	double ParserJson::parseDouble(std::string_view str) {
		return std::strtod(&str[0], nullptr);
	}
}