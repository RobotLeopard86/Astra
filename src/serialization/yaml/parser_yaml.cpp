#include "parser_yaml.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <memory>
#include <string>

#include "../define_retry.hpp"
#include "astra/expected.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/types/all_types.hpp"
#include "astra/variable/box.hpp"

using namespace astra;

ParserYaml::ParserYaml(const char* input, size_t inputSize)//
  : LexerYaml(input, inputSize),						   //
	_token(static_cast<char>(lex())) {
}

ParserYaml::ParserYaml(std::istream& stream)//
  : LexerYaml(stream),						//
	_token(static_cast<char>(lex())) {
}

Expected<None> ParserYaml::deserialize(TypeInfo* info) {
	while(true) {
		if(_token == 'S') {
			next();
			continue;
		}
		if(isEnd(_token)) {
			//break in the end of one document even if there are few in a file
			return Error("Unexpected end of file");
		}
		return parse(info);
	}
}

Expected<None> ParserYaml::parse(TypeInfo* info) {
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
		__retry(reflection::copy(info->var(), box.var()));

		next();

		return None();
	}

	//defining anchor
	if(_token == '&') {
		anchor = getWord().substr(1, anchor.size() - 1);
		next();
	}

	if(isNull(getWord())) {
		//do nothing, just skip the field
		next();
		return None();
	}

	if(info->is<Pointer>()) {
		auto p = info->unsafeGet<Pointer>();
		return p.getNested().matchMove(//
			[this, &p](const Error& /*err*/) -> Expected<None> {
				p.init();
				auto nestedInfo = reflection::reflect(p.var());
				return parse(&nestedInfo);
			},
			[this](Var var) -> Expected<None> {
				auto nestedInfo = reflection::reflect(var);
				return parse(&nestedInfo);
			});
	}

	Expected<None> ex = None();
	switch(_token) {
		case '-':
			ex = parseSeq(info);
			break;
		case '$':
			ex = parseStr(info);
			break;
		case '[':
			ex = info->match(
				[this](Array& a) -> Expected<None> {
					return parseFlowSeq(a.nestedType(), [&](size_t i, Var var) { return addToArray(a, i, var); });
				},
				[this](Sequence& s) -> Expected<None> {
					return parseFlowSeq(s.nestedType(), [&](size_t, Var var) { return s.push(var); });
				},
				[this](auto&&) -> Expected<None> { return errorMatch(); });
			break;
		case '{':
			ex = info->match([this](Object& o) -> Expected<None> { return parseFlowMap([&]() { return addToObj(o); }); },
				[this](Map& m) -> Expected<None> { return parseFlowMap(m); },
				[this](auto&&) -> Expected<None> { return errorMatch(); });
			break;
		case '?':
			ex = info->match([this](Map& m) -> Expected<None> { return parseMap(m); },
				[this](auto&&) -> Expected<None> {
					return error("A complex key, marked '?' could be deserialized in a map key only");
				});
			break;
		default:
			ex = errorToken(_token);
			break;
	}

	if(!anchor.empty()) {
		_anchors.emplace(anchor, Box(info->var().type()));

		reflection::copy(_anchors[anchor].var(), info->var());
	}

	return ex;
}

//parse string of "key: val ..."
Expected<None> ParserYaml::parseStr(TypeInfo* info) {
	//a string could be a key in a map
	//or name of a field in an object
	//or be just a string value of something
	auto ex = info->match(
		[this](Bool& b) -> Expected<None> {
			auto ex = b.set(parseBool(getWord()));
			next();
			return ex;
		},
		[this](Integer& i) -> Expected<None> {
			auto* p = i.var().rawMut();
			if(p == nullptr) {
				return Error("Trying to set const value");
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
			return None();
		},
		[this](Floating& f) -> Expected<None> {
			auto ex = f.set(parseDouble(getWord()));
			next();//skip '$' token
			return ex;
		},
		[this](String& s) -> Expected<None> {
			auto ex = s.set(getWord());
			next();//skip '$' token
			return ex;
		},
		[this](Enum& e) -> Expected<None> {
			auto ex = e.parse(getWord());
			next();//skip '$' token
			return ex;
		},
		[this](Map& m) -> Expected<None> { return parseMap(m); },
		[this](Object& o) -> Expected<None> { return parseMap([&]() { return addToObj(o); }); },
		[this](auto&&) -> Expected<None> { return errorMatch(); });
	__retry(ex);

	if(isEnd(_token)) {
		next();
	}

	return ex;
}

//parse "- val ..."
Expected<None> ParserYaml::parseSeq(TypeInfo* info) {
	return info->match(
		[this](Array& a) -> Expected<None> {
			return parseSeq(a.nestedType(), [&](size_t i, Var var) { return addToArray(a, i, var); });
		},
		[this](Sequence& s) -> Expected<None> {
			s.clear();
			return parseSeq(s.nestedType(), [&](size_t, Var var) { return s.push(var); });
		},
		[this](auto&& /*others*/) -> Expected<None> { return errorMatch(); });
}

Expected<None> ParserYaml::parseSeq(TypeId nestedType, std::function<Expected<None>(size_t, Var)> add) {
	if(_token == '[') {
		return parseFlowSeq(nestedType, std::move(add));
	}

	size_t i = 0;

	auto indFirst = getBorder();

	auto info = reflection::reflect(Var(nullptr, nestedType, false));
	while(!isEnd(_token)) {
		auto indNext = getBorder();
		if(indNext < indFirst) {
			break;
		}

		if(_token != '-') {
			return errorToken(_token);
		}
		next();//skip '-' itself

		Box box(nestedType);//Box should be a new object for each iteration
		info.unsafeAssign(box.var().rawMut());

		__retry(parse(&info));
		__retry(add(i, box.var()));
		i++;
	}
	return None();
}

//parse "[ val, ... ]"
Expected<None> ParserYaml::parseFlowSeq(TypeId nestedType, std::function<Expected<None>(size_t, Var)> add) {
	next();//skip '[' itself

	size_t i = 0;

	auto info = reflection::reflect(Var(nullptr, nestedType, false));
	while(!isEnd(_token) && _token != 'S' && _token != ']') {
		Box box(nestedType);//Box should be a new object for each iteration
		info.unsafeAssign(box.var().rawMut());

		__retry(parse(&info));
		i++;
		__retry(add(i, box.var()));

		if(_token == ',') {
			next();
		}
	}

	if(_token == ']') {
		next();
	}

	return None();
}

Expected<None> ParserYaml::addToArray(Array& a, size_t i, Var var) {
	if(i < a.size()) {
		auto item = a.at(i).unwrap();
		reflection::copy(item, var);
	}
	return None();
}

Expected<None> ParserYaml::parseMap(Map& map) {
	auto infoKey = reflection::reflect(Var(nullptr, map.keyType(), false));
	auto infoVal = reflection::reflect(Var(nullptr, map.valType(), false));

	map.clear();
	return parseMap([&]() { return addToMap(map, &infoKey, &infoVal); });
}

Expected<None> ParserYaml::parseMap(std::function<Expected<None>()> add) {
	if(_token == '{') {
		return parseFlowMap(std::move(add));
	}

	auto indFirst = getBorder();

	while(_token != 'S' && !isEnd(_token)) {
		auto indNext = getBorder();
		if(indNext < indFirst) {
			break;
		}

		__retry(add());
	}

	return None();
}

//parse "{ key:val, ... }" to an object
Expected<None> ParserYaml::parseFlowMap(std::function<Expected<None>()> add) {
	next();//skip '{' itself

	while(_token != 'S' && !isEnd(_token) && _token != '}') {

		if(_token == '$') {
			next();
		}

		if(_token == ':') {
			__retry(add());
		} else {
			return errorToken(_token);
		}

		if(_token == ',') {
			next();
		}
	}

	if(_token == '}') {
		next();
	}

	return None();
}

Expected<None> ParserYaml::parseFlowMap(Map& map) {
	auto infoKey = reflection::reflect(Var(nullptr, map.keyType(), false));
	auto infoVal = reflection::reflect(Var(nullptr, map.valType(), false));

	map.clear();
	return parseFlowMap([&]() { return addToMap(map, &infoKey, &infoVal); });
}

//map in YAML could be an object in C++
inline Expected<None> ParserYaml::addToObj(Object& obj) {
	if(_token == '$') {
		next();
	}

	auto ex = obj.getField(getWord());
	__retry(ex);

	if(_token != ':') {
		return errorToken(_token);
	}
	next();

	auto info = reflection::reflect(ex.unwrap().var());
	__retry(parse(&info));

	return None();
}

Expected<None> ParserYaml::addToMap(Map& map, TypeInfo* infoKey, TypeInfo* infoValue) {
	Box boxKey(map.keyType());
	infoKey->unsafeAssign(boxKey.var().rawMut());

	Box boxVal(map.valType());
	infoValue->unsafeAssign(boxVal.var().rawMut());

	//get a key
	if(_token == '$') {
		__retry(parseStr(infoKey));
	} else if(_token == '?') {
		next();//skip '?' token
		__retry(parse(infoKey));
	} else {
		return errorToken(_token);
	}

	if(_token != ':') {
		return errorToken(_token);
	}
	next();//skip ':' token

	//get a value
	__retry(parse(infoValue));

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

Error ParserYaml::error(const char* str) {
	return Error(astra::format("{}; {}", str, getPosition().toString()));
}

Error ParserYaml::errorToken(char token) {
	return Error(astra::format("Unexpected token '{}'; {}", token, getPosition().toString()));
}

Error ParserYaml::errorMatch() {
	return Error(astra::format("Cannot match correct type; {}", getPosition().toString()));
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
