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

#include "define_retry.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/types/all_types.hpp"
#include "astra/variable/box.hpp"

using namespace astra;

ParserJson::ParserJson(const char* input, std::size_t inputSize)//
  : LexerJson(input, inputSize) {
}

ParserJson::ParserJson(std::istream& stream)//
  : LexerJson(stream) {
}

Expected<None> ParserJson::deserialize(TypeInfo* info) {
	return parseNext(info);
}

Expected<None> ParserJson::parse(TypeInfo* info, char token) {
	if(getWord() == "null") {
		//do nothing, just skip the field
		return None();
	}

	if(info->is<Pointer>()) {
		auto p = info->unsafeGet<Pointer>();
		return p.getNested().matchMove(//
			[this, &p, token](const Error& /*err*/) -> Expected<None> {
				p.init();
				auto nestedInfo = reflection::reflect(p.var());
				return parse(&nestedInfo, token);
			},
			[this, token](Var var) -> Expected<None> {
				auto nestedInfo = reflection::reflect(var);
				return parse(&nestedInfo, token);
			});
	}

	switch(token) {
			// clang-format off
    case 's': {
      return info->match(
          [this](Bool& b) -> Expected<None> {  //
            return b.set(parseBool(getWord()));
          },
          [this](Integer& i) -> Expected<None> {
            auto w = getWord();

            if (w.front() == '-' || i.isSigned()) {
              return i.setSigned(std::strtoll(w.data(), nullptr, 10));
            }
            return i.setUnsigned(std::strtoull(w.data(), nullptr, 10));
          },
          [this](Floating& f) -> Expected<None> {  //
            return f.set(parseDouble(getWord()));
          },
          [this](auto&&) -> Expected<None> {  //
            return errorMatch();
          });
    }
    case '$':
      return info->match(
          [this](String& s) -> Expected<None> {
            return s.set(getWord());
          },
          [this](Enum& e) -> Expected<None> {
            return e.parse(getWord());
          },
          [this](Floating& f) -> Expected<None> {
            auto ex = parseDoubleSpecial(getWord());
            __retry(ex);
            return f.set(ex.unwrap());
          },
          [this](auto&&) -> Expected<None> {
            return errorMatch();
          });
    case '[':
      return info->match(
          [this](Array& a) -> Expected<None> {
            return parseArray(a.nestedType(), [&](std::size_t i, Var var) -> Expected<None> {
              if (i < a.size()) {
                auto item = a.at(i).unwrap();
                return reflection::copy(item, var);
              }
              return None();
            });
          },
          [this](Sequence& s) -> Expected<None> {
            s.clear();
            return parseArray(s.nestedType(), [&](std::size_t, Var var) {
              return s.push(var);
            });
          },
          [this](Map& m) -> Expected<None> {
            return parseMap(m);
          },
          [this](auto&&) -> Expected<None> {
            return errorMatch();
          });
		// clang-format on
		case '{':
			return parseObject(info);
		default:
			return errorToken(token);
	}
}

Expected<None> ParserJson::parseNext(TypeInfo* info) {
	next();
	return parse(info, _token);
}

Expected<None> ParserJson::parseArray(TypeId nestedType, std::function<Expected<None>(std::size_t, Var)> add) {
	next();//skip '['
	if(_token == ']') {
		//an empty array
		return None();
	}

	//save few ns for each iteration of the loop
	auto boxedInfo = reflection::reflect(Var(nullptr, nestedType, false));

	for(std::size_t i = 0; /**/; ++i) {
		Box box(nestedType);//Box should be a new object for each iteration
		boxedInfo.unsafeAssign(box.var().rawMut());

		auto ex = parse(&boxedInfo, _token)
					  .matchMove([&, i](None&&) -> Expected<None> { return add(i, box.var()); },
						  [](Error&& err) -> Expected<None> { return err; });
		__retry(ex);

		next();
		if(_token == ']') {
			return None();
		}
		if(_token != ',') {
			return errorToken(_token);
		}

		//get another one
		next();
	}

	return None();
}

Expected<None> ParserJson::parseObject(TypeInfo* info) {
	next();//skip '{'
	if(_token == '}') {
		//an empty object
		return None();
	}

	auto o = info->get<Object>();

	while(true) {
		if(_token != '$') {
			return error("Cannot reach a field name");
		}
		next();
		if(_token != ':') {
			return error("Cannot reach a field value");
		}

		auto field = o.getField(getWord()).unwrap().var();
		auto fieldInfo = reflection::reflect(field);
		__retry(parseNext(&fieldInfo));

		next();
		if(_token == '}') {
			return None();
		}
		if(_token != ',') {
			return errorToken(_token);
		}

		next();
	}

	return error("Max depth level exceeded");
}

Expected<None> ParserJson::parseMap(Map& map) {
	map.clear();

	next();//skip '['
	if(_token == ']') {
		//an empty map
		return None();
	}

	std::string key = "key";
	std::string val = "val";

	if(_token == '$') {
		//if particular tag found parse it
		auto pos = getWord().find("!!map");
		if(pos != std::string::npos) {

			auto kv = parseTag(getWord());
			__retry(kv);

			auto pair = kv.unwrap();
			key = std::move(pair.first);
			val = std::move(pair.second);

			//make step to get a new token
			next();
			if(_token != ',') {
				return errorToken(_token);
			}
			next();
		} else {
			return error("Cannot reach the map tag or '{'");
		}
	}

	else if(_token != '{') {
		return errorToken(_token);
	}

	auto keyInfo = reflection::reflect(Var(nullptr, map.keyType(), false));
	auto valInfo = reflection::reflect(Var(nullptr, map.valType(), false));

	//token '{' has already been read
	while(true) {
		Box keyBox(map.keyType());//Box should be a new object for each iteration
		keyInfo.unsafeAssign(keyBox.var().rawMut());

		Box valBox(map.valType());
		valInfo.unsafeAssign(valBox.var().rawMut());

		//parse first field key or val
		next();
		if(_token != '$') {
			return error("Cannot reach a field name");
		}

		next();
		if(_token != ':') {
			return error("Cannot reach a field value");
		}

		if(getWord() == key) {
			__retry(parseNext(&keyInfo));
		} else if(getWord() == val) {
			__retry(parseNext(&valInfo));
		} else {
			return Error(astra::format("Got an unexpected field '{}' while parse map; {}",//
				getWord(), getPosition().toString()));
		}

		next();
		if(_token == '}') {
			return error("Unexpected end of JSON object");
		}
		if(_token != ',') {
			return errorToken(_token);
		}

		//parse second field key or val
		next();
		if(_token != '$') {
			return error("Cannot reach a field name");
		}

		next();
		if(_token != ':') {
			return error("Cannot reach a field value");
		}

		if(getWord() == key) {
			__retry(parseNext(&keyInfo));
		} else if(getWord() == val) {
			__retry(parseNext(&valInfo));
		} else {
			return Error(astra::format("Got an unexpected field '{}' while parse map; {}",//
				getWord(), getPosition().toString()));
		}

		__retry(map.insert(keyBox.var(), valBox.var()));

		next();
		if(_token == '}') {
			next();
		}
		if(_token == ']') {
			return None();
		}
		if(_token != ',') {
			return errorToken(_token);
		}

		//take next '{'
		next();
	}
}

void ParserJson::next() {
	_token = static_cast<char>(lex());
}

Error ParserJson::error(const char* str) {
	return Error(astra::format("{}; {}", str, getPosition().toString()));
}

Error ParserJson::errorToken(char token) {
	return Error(astra::format("Unexpected token '{}'; {}", token, getPosition().toString()));
}

Error ParserJson::errorMatch() {
	return Error(astra::format("Cannot match correct type; {}", getPosition().toString()));
}

Expected<std::pair<std::string, std::string>> ParserJson::parseTag(std::string_view str) {
	auto pos1 = str.find('|');
	if(pos1 == std::string::npos) {
		return error("Cannot find '|' in the tag");
	}
	auto pos2 = str.find(':');
	if(pos2 == std::string::npos) {
		return error("Cannot find ':' in the tag");
	}

	auto key = std::string(str.substr(pos1 + 1, pos2 - (pos1 + 1)));
	auto val = std::string(str.substr(pos2 + 1, str.size() - (pos2 + 1)));

	return std::make_pair(std::move(key), std::move(val));
}

bool ParserJson::parseBool(std::string_view str) {
	return str != "false";
}

Expected<double> ParserJson::parseDoubleSpecial(std::string_view str) {
	if(str == "-inf") {
		return -std::numeric_limits<double>::infinity();
	}
	if(str == "inf") {
		return std::numeric_limits<double>::infinity();
	}
	if(str == "nan") {
		return std::nan("");
	}
	return Error(astra::format("Expected -inf, inf, nan but {} reached", str));
}

double ParserJson::parseDouble(std::string_view str) {
	return std::strtod(&str[0], nullptr);
}
