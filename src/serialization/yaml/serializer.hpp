#pragma once

#include <cmath>
#include <string_view>

#include "../writers/iwriter.hpp"
#include "astra/error/error.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/tools/stringify.hpp"
#include "astra/type_info/type_info.hpp"
#include "astra/type_info/variants/integer/integer.hpp"

namespace astra {

	inline void append(IWriter* writer, std::string_view str) {
		writer->write(str.data(), str.size());
	}

	inline void append(IWriter* writer, char ch) {
		writer->write(ch);
	}

	inline void appendIndention(IWriter* writer, int num) {
		for(auto i = 0; i < num; i++) {
			writer->write(' ');
		}
	}

	inline bool isComplex(const TypeInfo& info) {
		switch(info.getKind()) {
			case TypeInfo::Kind::kObject:
				[[fallthrough]];
			case TypeInfo::Kind::kArray:
				[[fallthrough]];
			case TypeInfo::Kind::kSequence:
				[[fallthrough]];
			case TypeInfo::Kind::kMap:
				return true;
			case TypeInfo::Kind::kPointer: {
				auto p = info.unsafeGet<Pointer>();
				return p.getNested().matchMove(//
					[](const Error& /*err*/) { return false; },
					[](Var var) {
						auto nestedInfo = reflection::reflect(var);
						return isComplex(nestedInfo);
					});
			}
			default:
				return false;
		}
	}

	template<typename SeqT>
	inline void serializeSequence(const SeqT& seq, IWriter* writer, int indent);

	std::string doubleToString(double value) {
		if(value == -std::numeric_limits<double>::infinity()) {
			return "!!float .-inf";
		}
		if(value == std::numeric_limits<double>::infinity()) {
			return "!!float .inf";
		}
		if(std::isnan(value)) {
			return "!!float .nan";
		}
		return toString(value, 9);
	}

	void serializeRecursive(IWriter* writer, const TypeInfo& info, int indent) {
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
				append(writer, info.unsafeGet<String>().get());
				break;
			case TypeInfo::Kind::kEnum:
				append(writer, info.unsafeGet<Enum>().toString());
				break;
			case TypeInfo::Kind::kObject: {
				const auto& o = info.unsafeGet<Object>();

				bool isFirst = true;
				for(auto&& record : o.getFields()) {

					if(!isFirst) {
						appendIndention(writer, indent);
					}
					isFirst = false;

					append(writer, record.first);

					auto fieldInfo = reflection::reflect(record.second.var());

					if(isComplex(fieldInfo)) {
						append(writer, ":\n");
						append(writer, std::string(indent + 2, ' '));
						serializeRecursive(writer, fieldInfo, indent + 2);

					} else {
						append(writer, ": ");
						serializeRecursive(writer, fieldInfo, indent + 2);
					}

					append(writer, '\n');
				}
				writer->stepBack(1);
				break;
			}
			case TypeInfo::Kind::kMap: {
				//TODO clear this hell
				const auto& m = info.unsafeGet<Map>();

				if(m.size() == 0) {
					append(writer, "{}\n");
					return;
				}

				const auto serComplexKey = [](IWriter* writer, int indent, const TypeInfo& info) {
					append(writer, "? ");
					serializeRecursive(writer, info, indent + 2);
					append(writer, "\n");
					appendIndention(writer, indent);
				};

				const auto serNormalKey = [](IWriter* writer, int indent, const TypeInfo& info) {
					serializeRecursive(writer, info, indent);
				};

				const auto serComplexVal = [](IWriter* writer, int indent, const TypeInfo& info) {
					append(writer, ":\n");
					append(writer, std::string(indent + 2, ' '));
					serializeRecursive(writer, info, indent + 2);
				};

				const auto serNormalVal = [](IWriter* writer, int indent, const TypeInfo& info) {
					append(writer, ": ");
					serializeRecursive(writer, info, indent + 2);
				};

				auto keyInfo = reflection::reflect(Var(nullptr, m.keyType(), false));
				auto valInfo = reflection::reflect(Var(nullptr, m.valType(), false));

				bool isFirst = true;
				m.unsafeForEach([writer,													//
									indent,													//
									&keyInfo, &valInfo,										//
									&isFirst,												//
									serComplexKey, serNormalKey, serComplexVal, serNormalVal//
				](void* key, void* value) {
					if(!isFirst) {
						appendIndention(writer, indent);
					}
					isFirst = false;

					keyInfo.unsafeAssign(key);
					valInfo.unsafeAssign(value);

					void (*serVal)(IWriter* writer, int indent, const TypeInfo& info);
					void (*serKey)(IWriter* writer, int indent, const TypeInfo& info);

					if(isComplex(keyInfo)) {
						serKey = serComplexKey;
						serVal = serNormalVal;
					} else {
						serKey = serNormalKey;
						if(isComplex(valInfo)) {
							serVal = serComplexVal;
						} else {
							serVal = serNormalVal;
						}
					}

					serKey(writer, indent, keyInfo);
					serVal(writer, indent, valInfo);

					append(writer, '\n');
				});
				writer->stepBack(1);
				break;
			}
			case TypeInfo::Kind::kArray:
				serializeSequence(info.unsafeGet<Array>(), writer, indent);
				break;
			case TypeInfo::Kind::kSequence:
				serializeSequence(info.unsafeGet<Sequence>(), writer, indent);
				break;
			case TypeInfo::Kind::kPointer: {
				auto p = info.unsafeGet<Pointer>();
				p.getNested().matchMove(//
					[writer](const Error& /*err*/) { append(writer, "null"); },
					[writer, indent](Var var) {
						auto info = reflection::reflect(var);
						serializeRecursive(writer, info, indent);
					});
			} break;
		}
	}

	template<typename SeqT>
	inline void serializeSequence(const SeqT& seq, IWriter* writer, int indent) {
		//TODO add possibility to write short sequences in one line
		if(seq.size() == 0) {
			append(writer, "[]\n");
			return;
		}

		auto info = reflection::reflect(Var(nullptr, seq.nestedType(), false));

		bool isFirst = true;
		seq.unsafeForEach([writer, indent, &info, &isFirst](void* ptr) {
			if(!isFirst) {
				appendIndention(writer, indent);
			}
			isFirst = false;

			append(writer, "- ");

			info.unsafeAssign(ptr);
			serializeRecursive(writer, info, indent + 2);

			append(writer, '\n');
		});
		writer->stepBack(1);
	}

}
