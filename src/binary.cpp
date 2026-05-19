#include "astra/serialization/binary.hpp"

#include <iostream>

#include "stream_reader.hpp"
#include "vector_reader.hpp"
#include "stream_writer.hpp"
#include "vector_writer.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/types/all_types.hpp"
#include "astra/variable/box.hpp"
#include "group_reader.hpp"
#include "group_writer.hpp"

using namespace astra::serialization;

template<typename SeqT>
inline void serializeSequence(const SeqT& seq, GroupWriter* writer);

inline void serializeRecursive(GroupWriter* writer, const TypeInfo& info) {
	auto k = info.getKind();

	switch(k) {
		case TypeInfo::Kind::kObject:
			for(auto&& record : info.unsafeGet<Object>().getFields()) {
				//skip name in record.first;
				auto fieldInfo = reflection::reflect(record.second.var());
				serializeRecursive(writer, fieldInfo);
			}
			break;
		case TypeInfo::Kind::kBool:
			writer->write(info.unsafeGet<Bool>().get());
			break;
		case TypeInfo::Kind::kInteger: {
			auto i = info.unsafeGet<Integer>();
			writer->write(i.var().raw(), i.size(), i.isSigned());
		} break;
		case TypeInfo::Kind::kFloating:
			writer->write(info.unsafeGet<Floating>().get());
			break;
		case TypeInfo::Kind::kString:
			writer->write(info.unsafeGet<String>().get());
			break;
		case TypeInfo::Kind::kEnum:
			writer->write(info.unsafeGet<Enum>().toString());
			break;
		case TypeInfo::Kind::kMap: {
			auto m = info.unsafeGet<Map>();

			writer->write(m.size());

			auto keyInfo = reflection::reflect(Var(nullptr, m.keyType(), false));
			auto valInfo = reflection::reflect(Var(nullptr, m.valType(), false));
			m.unsafeForEach([writer, &keyInfo, &valInfo](void* key, void* val) {
				keyInfo.unsafeAssign(key);
				serializeRecursive(writer, keyInfo);

				valInfo.unsafeAssign(val);
				serializeRecursive(writer, valInfo);
			});
		} break;
		case TypeInfo::Kind::kArray:
			serializeSequence(info.unsafeGet<Array>(), writer);
			break;
		case TypeInfo::Kind::kSequence:
			serializeSequence(info.unsafeGet<Sequence>(), writer);
			break;
		case TypeInfo::Kind::kPointer: {
			auto p = info.unsafeGet<Pointer>();
			p.getNested().matchMove(//
				[writer](const Error& /*err*/) { writer->writeNull(); },
				[writer](Var var) {
					auto info = reflection::reflect(var);
					serializeRecursive(writer, info);
				});
		} break;
	}
}

template<typename SeqT>
inline void serializeSequence(const SeqT& seq, GroupWriter* writer) {
	writer->write(seq.size());

	auto info = reflection::reflect(Var(nullptr, seq.nestedType(), false));
	seq.unsafeForEach([writer, &info](void* ptr) {
		info.unsafeAssign(ptr);
		serializeRecursive(writer, info);
	});
}

void binary::serialize(std::vector<uint8_t>* vector, Var var) {
	VectorWriter vectorW(vector);
	GroupWriter groupW(&vectorW);

	auto info = reflection::reflect(var);

	serializeRecursive(&groupW, info);
}

void binary::serialize(std::ostream& stream, Var var) {
	StreamWriter streamW(stream);
	GroupWriter groupW(&streamW);

	auto info = reflection::reflect(var);

	serializeRecursive(&groupW, info);
}

inline void deserializeRecursive(TypeInfo* info, const GroupReader& reader) {
	auto k = info->getKind();

	switch(k) {
		case TypeInfo::Kind::kObject:
			for(auto&& record : info->unsafeGet<Object>().getFields()) {
				//skip name in record.first;
				auto fieldInfo = reflection::reflect(record.second.var());
				deserializeRecursive(&fieldInfo, reader);
			}
			break;
		case TypeInfo::Kind::kBool:
			info->unsafeGet<Bool>().set(reader.readUnsigned() == 1);
			break;
		case TypeInfo::Kind::kInteger: {
			auto i = info->unsafeGet<Integer>();
			if(i.isSigned()) {
				i.setSigned(reader.readSigneg());
			} else {
				i.setUnsigned(reader.readUnsigned());
			}
		} break;
		case TypeInfo::Kind::kFloating:
			info->unsafeGet<Floating>().set(reader.readFloat());
			break;
		case TypeInfo::Kind::kString:
			info->unsafeGet<String>().set(reader.readString());
			break;
		case TypeInfo::Kind::kEnum:
			info->unsafeGet<Enum>().parse(reader.readString());
			break;
		case TypeInfo::Kind::kMap: {
			auto m = info->unsafeGet<Map>();
			m.clear();

			auto keyInfo = reflection::reflect(Var(nullptr, m.keyType(), false));
			auto valInfo = reflection::reflect(Var(nullptr, m.valType(), false));

			auto n = reader.readUnsigned();
			for(auto i = 0; i < n; i++) {
				Box keyBox(m.keyType());//Box should be a new object for each iteration
				keyInfo.unsafeAssign(keyBox.var().rawMut());

				deserializeRecursive(&keyInfo, reader);

				Box valBox(m.valType());
				valInfo.unsafeAssign(valBox.var().rawMut());

				deserializeRecursive(&valInfo, reader);

				m.insert(keyBox.var(), valBox.var());
			}
		} break;
		case TypeInfo::Kind::kArray: {
			auto a = info->unsafeGet<Array>();
			auto n = reader.readUnsigned();

			std::size_t i = 0;
			a.forEach([&reader, &i, n](Var entry) {
				if(i >= n) {
					return;
				}
				i++;

				auto entryInfo = reflection::reflect(entry);
				deserializeRecursive(&entryInfo, reader);
			});
		} break;
		case TypeInfo::Kind::kSequence: {
			auto s = info->unsafeGet<Sequence>();
			s.clear();

			auto entryInfo = reflection::reflect(Var(nullptr, s.nestedType(), false));

			auto n = reader.readUnsigned();
			for(auto i = 0; i < n; i++) {
				Box entryBox(s.nestedType());//Box should be a new object for each iteration
				entryInfo.unsafeAssign(entryBox.var().rawMut());

				deserializeRecursive(&entryInfo, reader);

				s.push(entryBox.var());
			}
		} break;
		case TypeInfo::Kind::kPointer: {
			if(reader.isNull()) {
				reader.readUnsigned();//skip a byte
				return;
			}
			auto p = info->unsafeGet<Pointer>();
			p.getNested().matchMove(//
				[&reader, &p](const Error& /*err*/) {
					p.init();
					auto nestedInfo = reflection::reflect(p.var());
					deserializeRecursive(&nestedInfo, reader);
				},
				[&reader](Var var) {
					auto nestedInfo = reflection::reflect(var);
					deserializeRecursive(&nestedInfo, reader);
				});
		} break;
	}
}

Expected<None> binary::deserialize(Var var, const std::vector<uint8_t>& vector) {
	VectorReader vectorR(vector);
	GroupReader groupR(&vectorR);

	auto info = reflection::reflect(var);

	deserializeRecursive(&info, groupR);

	return None();
}

Expected<None> binary::deserialize(Var var, std::istream& stream) {
	StreamReader streamR(stream);
	GroupReader groupR(&streamR);

	auto info = reflection::reflect(var);

	deserializeRecursive(&info, groupR);

	return None();
}
