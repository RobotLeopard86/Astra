#include "astra/serialization/json.hpp"

#include <cmath>
#include <string_view>

#include "iwriter.hpp"
#include "stream_writer.hpp"
#include "string_writer.hpp"
#include "astra/error/error.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/tools/stringify.hpp"
#include "astra/type_info/type_info.hpp"
#include "astra/types/all_types.hpp"
#include "parser_json.hpp"

using namespace astra::serialization;

inline void append(IWriter* writer, std::string_view str) {
	writer->write(str.data(), str.size());
}

inline void append(IWriter* writer, char ch) {
	writer->write(ch);
}

template<typename SeqT>
inline void serializeSequence(const SeqT& seq, IWriter* writer);

std::string doubleToString(double value) {
	if(value == -std::numeric_limits<double>::infinity()) {
		return "\"-inf\"";
	}
	if(value == std::numeric_limits<double>::infinity()) {
		return "\"inf\"";
	}
	if(std::isnan(value)) {
		return "\"nan\"";
	}
	return toString(value, 9);
}

void serializeRecursive(IWriter* writer, const TypeInfo& info) {
	auto k = info.getKind();

	switch(k) {
		case TypeInfo::Kind::kBool:
			append(writer, toString(info.unsafeGet<Bool>().get()));
			break;
		case TypeInfo::Kind::kInteger: {
			auto i = info.unsafeGet<Integer>();
			if(i.isSigned()) {
				append(writer, toString(i.asSigned()));
			} else {
				append(writer, toString(i.asUnsigned()));
			}
		} break;
		case TypeInfo::Kind::kFloating:
			append(writer, doubleToString(info.unsafeGet<Floating>().get()));
			break;
		case TypeInfo::Kind::kString:
			append(writer, '"');
			append(writer, info.unsafeGet<String>().get());
			append(writer, '"');
			break;
		case TypeInfo::Kind::kEnum:
			append(writer, '"');
			append(writer, info.unsafeGet<Enum>().toString());
			append(writer, '"');
			break;
		case TypeInfo::Kind::kObject: {
			const auto& o = info.unsafeGet<Object>();

			auto fields = o.getFields();

			if(fields.size() == 0) {
				append(writer, "{}");
				return;
			}
			append(writer, '{');

			for(auto&& record : fields) {
				append(writer, '"');
				append(writer, record.first);
				append(writer, "\":");

				auto fieldInfo = reflection::reflect(record.second.var());
				serializeRecursive(writer, fieldInfo);
				append(writer, ',');
			}
			writer->stepBack(1);
			append(writer, '}');

		} break;
		case TypeInfo::Kind::kArray:
			serializeSequence(info.unsafeGet<Array>(), writer);
			break;
		case TypeInfo::Kind::kSequence:
			serializeSequence(info.unsafeGet<Sequence>(), writer);
			break;
		case TypeInfo::Kind::kMap: {
			const auto& m = info.unsafeGet<Map>();

			if(m.size() == 0) {
				append(writer, "[]");
				return;
			}
			append(writer, '[');

			auto keyInfo = reflection::reflect(Var(nullptr, m.keyType(), false));
			auto valInfo = reflection::reflect(Var(nullptr, m.valType(), false));
			m.unsafeForEach([writer, &keyInfo, &valInfo](void* key, void* val) {
				append(writer, "{\"key\":");
				keyInfo.unsafeAssign(key);
				serializeRecursive(writer, keyInfo);

				append(writer, ',');

				append(writer, "\"val\":");
				valInfo.unsafeAssign(val);
				serializeRecursive(writer, valInfo);

				append(writer, "},");
			});
			writer->stepBack(1);
			append(writer, ']');
		} break;
		case TypeInfo::Kind::kPointer: {
			auto p = info.unsafeGet<Pointer>();
			p.getNested().matchMove(//
				[writer](const Error& /*err*/) { append(writer, "null"); },
				[writer](Var var) {
					auto info = reflection::reflect(var);
					serializeRecursive(writer, info);
				});
		} break;
	}
}

template<typename SeqT>
inline void serializeSequence(const SeqT& seq, IWriter* writer) {
	if(seq.size() == 0) {
		append(writer, "[]");
		return;
	}
	append(writer, '[');

	auto info = reflection::reflect(Var(nullptr, seq.nestedType(), false));
	seq.unsafeForEach([writer, &info](void* ptr) {
		info.unsafeAssign(ptr);
		serializeRecursive(writer, info);

		append(writer, ',');
	});
	writer->stepBack(1);
	append(writer, ']');
}

void json::serialize(std::string* str, Var var) {
	StringWriter stringW(str);
	auto info = reflection::reflect(var);

	serializeRecursive(&stringW, info);
}

void json::serialize(std::ostream& stream, Var var) {
	StreamWriter streamW(stream);
	auto info = reflection::reflect(var);

	serializeRecursive(&streamW, info);
}

Expected<None> json::deserialize(Var var, std::string_view str) {
	ParserJson parser(str.data(), str.size());
	auto info = reflection::reflect(var);

	return parser.deserialize(&info);
}

Expected<None> json::deserialize(Var var, std::istream& stream) {
	ParserJson parser(stream);
	auto info = reflection::reflect(var);

	return parser.deserialize(&info);
}