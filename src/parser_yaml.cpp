#include "parser_yaml.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <memory>
#include <string>



#include "astra/reflection.hpp"
#include "astra/types/all_types.hpp"
#include "astra/box.hpp"

namespace astra {

	ParserYaml::ParserYaml(const char* input, std::size_t inputSize)//
	  : LexerYaml(input, inputSize),								//
		_token(static_cast<char>(lex())) {
	}

	ParserYaml::ParserYaml(std::istream& stream)//
	  : LexerYaml(stream),						//
		_token(static_cast<char>(lex())) {
	}

	void ParserYaml::deserialize(TypeInfo* info) {
		while(true) {
			if(_token == 'S') {
				next();
				continue;
			}
			if(isEnd(_token)) {
				//break in the end of one document even if there are few in a file
				throw std::runtime_error("Unexpected end of file");
			}
			return parse(info);
		}
	}

	void ParserYaml::parse(TypeInfo* info) {
		//just ignore tags
		if(_token == '!') {
			next();
		}

		std::string anchor;
		//dereference anchor
		if(_token == '*') {
			anchor = getWord().substr(1, anchor.size() - 1);
			auto& box = _anchors[anchor];

			//there is a type check
			copy(info->var(), box.var());

			next();

			return;
		}

		//defining anchor
		if(_token == '&') {
			anchor = getWord().substr(1, anchor.size() - 1);
			next();
		}

		if(isNull(getWord())) {
			//do nothing, just skip the field
			next();
			return;
		}

		if(info->is<Pointer>()) {
			auto p = info->asUnsafe<Pointer>();
			try {
				Var var = p.getNested();
				auto nestedInfo = reflect(var);
				return parse(&nestedInfo);
			} catch(...) {
				p.init();
				auto nestedInfo = reflect(p.var());
				return parse(&nestedInfo);
			}
		}
		switch(_token) {
			case '-':
				parseSeq(info);
				break;
			case '$':
				parseStr(info);
				break;
			case '[':
				info->match(
					[this](Array& a) -> void {
						return parseFlowSeq(a.nestedType(), [&](std::size_t i, Var var) { return addToArray(a, i, var); });
					},
					[this](List& s) -> void {
						return parseFlowSeq(s.nestedType(), [&](std::size_t, Var var) { return s.push(var); });
					},
					[this](auto&&) -> void { errorMatch(); });
				break;
			case '{':
				info->match([this](Object& o) -> void { return parseFlowMap([&]() { return addToObj(o); }); },
					[this](Map& m) -> void { return parseFlowMap(m); },
					[this](auto&&) -> void { errorMatch(); });
				break;
			case '?':
				info->match([this](Map& m) -> void { return parseMap(m); },
					[this](auto&&) -> void {
						error("A complex key, marked '?' could be deserialized in a map key only");
					});
				break;
			default:
				errorToken(_token);
				break;
		}

		if(!anchor.empty()) {
			_anchors.emplace(anchor, Box(info->var().type()));

			copy(_anchors[anchor].var(), info->var());
		}
	}

	//parse string of "key: val ..."
	void ParserYaml::parseStr(TypeInfo* info) {
		//a string could be a key in a map
		//or name of a field in an object
		//or be just a string value of something
		info->match(
			[this](Bool& b) -> void {
				b.set(parseBool(getWord()));
				next();
			},
			[this](Integer& i) -> void {
				auto* p = i.var().rawMut();
				if(p == nullptr) {
					throw std::runtime_error("Trying to set const value");
				}

				auto w = getWord();
				if(w.front() == '-') {
					auto v = std::strtoll(&w[0], nullptr, 10);
					std::memcpy(p, &v, i.size());
				} else {
					auto v = std::strtoull(&w[0], nullptr, 10);
					std::memcpy(p, &v, i.size());
				}

				next();
			},
			[this](Floating& f) -> void {
				f.set(parseDouble(getWord()));
				next();//skip '$' token
			},
			[this](String& s) -> void {
				s.set(getWord());
				next();//skip '$' token
			},
			[this](Enum& e) -> void {
				e.fromString(getWord());
				next();//skip '$' token
			},
			[this](Map& m) -> void { parseMap(m); },
			[this](Object& o) -> void { parseMap([&]() { return addToObj(o); }); },
			[this](auto&&) -> void { errorMatch(); });

		if(isEnd(_token)) {
			next();
		}
	}

	//parse "- val ..."
	void ParserYaml::parseSeq(TypeInfo* info) {
		return info->match(
			[this](Array& a) -> void {
				return parseSeq(a.nestedType(), [&](std::size_t i, Var var) { return addToArray(a, i, var); });
			},
			[this](List& s) -> void {
				s.clear();
				return parseSeq(s.nestedType(), [&](std::size_t, Var var) { return s.push(var); });
			},
			[this](auto&& /*others*/) -> void { errorMatch(); });
	}

	void ParserYaml::parseSeq(TypeId nestedType, std::function<void(std::size_t, Var)> add) {
		if(_token == '[') {
			return parseFlowSeq(nestedType, std::move(add));
		}

		std::size_t i = 0;

		auto indFirst = getBorder();

		auto info = reflect(Var(nullptr, nestedType, false));
		while(!isEnd(_token)) {
			auto indNext = getBorder();
			if(indNext < indFirst) {
				break;
			}

			if(_token != '-') {
				errorToken(_token);
			}
			next();//skip '-' itself

			Box box(nestedType);//Box should be a new object for each iteration
			info.unsafeAssign(box.var().rawMut());

			parse(&info);
			add(i, box.var());
			i++;
		}
		return;
	}

	//parse "[ val, ... ]"
	void ParserYaml::parseFlowSeq(TypeId nestedType, std::function<void(std::size_t, Var)> add) {
		next();//skip '[' itself

		std::size_t i = 0;

		auto info = reflect(Var(nullptr, nestedType, false));
		while(!isEnd(_token) && _token != 'S' && _token != ']') {
			Box box(nestedType);//Box should be a new object for each iteration
			info.unsafeAssign(box.var().rawMut());

			parse(&info);
			i++;
			add(i, box.var());

			if(_token == ',') {
				next();
			}
		}

		if(_token == ']') {
			next();
		}

		return;
	}

	void ParserYaml::addToArray(Array& a, std::size_t i, Var var) {
		if(i < a.size()) {
			auto item = a.at(i);
			copy(item, var);
		}
		return;
	}

	void ParserYaml::parseMap(Map& map) {
		auto infoKey = reflect(Var(nullptr, map.keyType(), false));
		auto infoVal = reflect(Var(nullptr, map.valType(), false));

		map.clear();
		return parseMap([&]() { return addToMap(map, &infoKey, &infoVal); });
	}

	void ParserYaml::parseMap(std::function<void()> add) {
		if(_token == '{') {
			return parseFlowMap(std::move(add));
		}

		auto indFirst = getBorder();

		while(_token != 'S' && !isEnd(_token)) {
			auto indNext = getBorder();
			if(indNext < indFirst) {
				break;
			}

			add();
		}

		return;
	}

	//parse "{ key:val, ... }" to an object
	void ParserYaml::parseFlowMap(std::function<void()> add) {
		next();//skip '{' itself

		while(_token != 'S' && !isEnd(_token) && _token != '}') {

			if(_token == '$') {
				next();
			}

			if(_token == ':') {
				add();
			} else {
				errorToken(_token);
			}

			if(_token == ',') {
				next();
			}
		}

		if(_token == '}') {
			next();
		}

		return;
	}

	void ParserYaml::parseFlowMap(Map& map) {
		auto infoKey = reflect(Var(nullptr, map.keyType(), false));
		auto infoVal = reflect(Var(nullptr, map.valType(), false));

		map.clear();
		return parseFlowMap([&]() { return addToMap(map, &infoKey, &infoVal); });
	}

	//map in YAML could be an object in C++
	inline void ParserYaml::addToObj(Object& obj) {
		if(_token == '$') {
			next();
		}

		auto ex = obj.getField(getWord());
		if(_token != ':') {
			errorToken(_token);
		}
		next();

		auto info = reflect(ex.var());
		parse(&info);

		return;
	}

	void ParserYaml::addToMap(Map& map, TypeInfo* infoKey, TypeInfo* infoValue) {
		Box boxKey(map.keyType());
		infoKey->unsafeAssign(boxKey.var().rawMut());

		Box boxVal(map.valType());
		infoValue->unsafeAssign(boxVal.var().rawMut());

		//get a key
		if(_token == '$') {
			parseStr(infoKey);
		} else if(_token == '?') {
			next();//skip '?' token
			parse(infoKey);
		} else {
			errorToken(_token);
		}

		if(_token != ':') {
			errorToken(_token);
		}
		next();//skip ':' token

		//get a value
		parse(infoValue);

		return map.insert(boxKey.var(), boxVal.var());
	}

	void ParserYaml::next() {
		if(_token != 0) {
			_token = static_cast<char>(lex());
		}
	}

	bool ParserYaml::isEnd(int token) {
		return token == 0 || token == 'E';
	}

	bool ParserYaml::isNull(const std::string& word) {
		return word == "null" || word == "Null" || word == "NULL" || word == "~";
	}

	void ParserYaml::error(const char* str) {
		throw std::runtime_error(::astra::format("{}; {}", str, getPosition().toString()));
	}

	void ParserYaml::errorToken(char token) {
		throw std::runtime_error(::astra::format("Unexpected token '{}'; {}", token, getPosition().toString()));
	}

	void ParserYaml::errorMatch() {
		throw std::runtime_error(::astra::format("Cannot match correct type; {}", getPosition().toString()));
	}

	bool ParserYaml::parseBool(std::string& str) {
		std::transform(str.data(), str.data() + str.length(), str.data(), [](char c) { return std::tolower(c); });
		return !(str == "false" || str == "off" || str == "no" || str == "n");
	}

	double ParserYaml::parseDouble(std::string_view str) {
		if(str == ".-inf") {
			return -std::numeric_limits<double>::infinity();
		}
		if(str == ".inf") {
			return std::numeric_limits<double>::infinity();
		}
		if(str == ".nan") {
			return std::nan("");
		}
		return std::strtod(&str[0], nullptr);
	}
}