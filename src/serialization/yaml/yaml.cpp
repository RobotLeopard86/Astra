#include "astra/serialization/yaml.hpp"

#include <ostream>

#include "../writers/stream_writer.hpp"
#include "../writers/string_writer.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/types/all_types.hpp"
#include "parser_yaml.hpp"
#include "serializer.hpp"

using namespace astra::serialization;

void yaml::serialize(std::string* str, Var var) {
	StringWriter stringW(str);
	auto info = reflection::reflect(var);

	serializeRecursive(&stringW, info, 0);
}

void yaml::serialize(std::ostream& stream, Var var) {
	StreamWriter streamW(stream);
	auto info = reflection::reflect(var);

	serializeRecursive(&streamW, info, 0);
}

Expected<None> yaml::deserialize(Var var, std::string_view str) {
	ParserYaml parser(str.data(), str.size());
	auto info = reflection::reflect(var);

	return parser.deserialize(&info);
}

Expected<None> yaml::deserialize(Var var, std::istream& stream) {
	ParserYaml parser(stream);
	auto info = reflection::reflect(var);

	return parser.deserialize(&info);
}