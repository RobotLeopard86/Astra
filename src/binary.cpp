#include "astra/binary.hpp"

#include <iostream>
#include <memory>

#include "astra/reflection.hpp"
#include "astra/types/all_types.hpp"// IWYU pragma: keep
#include "bytestream.hpp"
#include "libjaguar/Document.hpp"

namespace astra {

	/*template<typename SeqT>
	inline void serializeSequence(const SeqT& seq, GroupWriter* writer);

	inline void serializeRecursive(GroupWriter* writer, const TypeInfo& info) {
		auto k = info.getKind();

		switch(k) {
			case TypeInfo::Kind::kObject:
				for(auto&& record : info.asUnsafe<Object>().getFields()) {
					//skip name in record.first;
					auto fieldInfo = reflect(record.second.var());
					serializeRecursive(writer, fieldInfo);
				}
				break;
			case TypeInfo::Kind::kBool:
				writer->write(info.asUnsafe<Bool>().get());
				break;
			case TypeInfo::Kind::kInteger: {
				auto i = info.asUnsafe<Integer>();
				writer->write(i.var().raw(), i.size(), i.isSigned());
			} break;
			case TypeInfo::Kind::kFloating:
				writer->write(info.asUnsafe<Floating>().get());
				break;
			case TypeInfo::Kind::kString:
				writer->write(info.asUnsafe<String>().get());
				break;
			case TypeInfo::Kind::kEnum:
				writer->write(info.asUnsafe<Enum>().toString());
				break;
			case TypeInfo::Kind::kMap: {
				auto m = info.asUnsafe<Map>();

				writer->write(m.size());

				auto keyInfo = reflect(Var(nullptr, m.keyType(), false));
				auto valInfo = reflect(Var(nullptr, m.valType(), false));
				m.unsafeForEach([writer, &keyInfo, &valInfo](void* key, void* val) {
					keyInfo.unsafeAssign(key);
					serializeRecursive(writer, keyInfo);

					valInfo.unsafeAssign(val);
					serializeRecursive(writer, valInfo);
				});
			} break;
			case TypeInfo::Kind::kArray:
				serializeSequence(info.asUnsafe<Array>(), writer);
				break;
			case TypeInfo::Kind::kSequence:
				serializeSequence(info.asUnsafe<Sequence>(), writer);
				break;
			case TypeInfo::Kind::kPointer: {
				auto p = info.asUnsafe<Pointer>();
				try {
					Var var = p.getNested();
					auto info = reflect(var);
					serializeRecursive(writer, info);
				} catch(...) {
					writer->writeNull();
				}
			} break;
		}
	}

	template<typename SeqT>
	inline void serializeSequence(const SeqT& seq, GroupWriter* writer) {
		writer->write(seq.size());

		auto info = reflect(Var(nullptr, seq.nestedType(), false));
		seq.unsafeForEach([writer, &info](void* ptr) {
			info.unsafeAssign(ptr);
			serializeRecursive(writer, info);
		});
	}*/

	/*inline void deserializeRecursive(TypeInfo* info, const GroupReader& reader) {
		auto k = info->getKind();

		switch(k) {
			case TypeInfo::Kind::kObject:
				for(auto&& record : info->asUnsafe<Object>().getFields()) {
					//skip name in record.first;
					auto fieldInfo = reflect(record.second.var());
					deserializeRecursive(&fieldInfo, reader);
				}
				break;
			case TypeInfo::Kind::kBool:
				info->asUnsafe<Bool>().set(reader.readUnsigned() == 1);
				break;
			case TypeInfo::Kind::kInteger: {
				auto i = info->asUnsafe<Integer>();
				if(i.isSigned()) {
					i.setSigned(reader.readSigneg());
				} else {
					i.setUnsigned(reader.readUnsigned());
				}
			} break;
			case TypeInfo::Kind::kFloating:
				info->asUnsafe<Floating>().set(reader.readFloat());
				break;
			case TypeInfo::Kind::kString:
				info->asUnsafe<String>().set(reader.readString());
				break;
			case TypeInfo::Kind::kEnum:
				info->asUnsafe<Enum>().fromString(reader.readString());
				break;
			case TypeInfo::Kind::kMap: {
				auto m = info->asUnsafe<Map>();
				m.clear();

				auto keyInfo = reflect(Var(nullptr, m.keyType(), false));
				auto valInfo = reflect(Var(nullptr, m.valType(), false));

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
				auto a = info->asUnsafe<Array>();
				auto n = reader.readUnsigned();

				std::size_t i = 0;
				a.forEach([&reader, &i, n](Var entry) {
					if(i >= n) {
						return;
					}
					i++;

					auto entryInfo = reflect(entry);
					deserializeRecursive(&entryInfo, reader);
				});
			} break;
			case TypeInfo::Kind::kSequence: {
				auto s = info->asUnsafe<Sequence>();
				s.clear();

				auto entryInfo = reflect(Var(nullptr, s.nestedType(), false));

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
				auto p = info->asUnsafe<Pointer>();
				try {
					Var var = p.getNested();
					TypeInfo nestedInfo = reflect(var);
					deserializeRecursive(&nestedInfo, reader);
				} catch(...) {
					p.init();
					TypeInfo nestedInfo = reflect(p.var());
					deserializeRecursive(&nestedInfo, reader);
				}
			} break;
		}
	}*/

	void binary::serialize(std::vector<uint8_t>& vector, Var var) {
		//Forward via stream
		obytestream obs(vector);
		serialize(obs, var);
	}

	void binary::deserialize(Var var, const std::vector<uint8_t>& vector) {
		//Forward via stream
		ibytestream ibs(const_cast<std::vector<uint8_t>&>(vector));
		deserialize(var, ibs);
	}

	void binary::serialize(std::ostream& stream, Var var) {
		//Setup Jaguar document
		libjaguar::Document jdoc;

		//Business logic...
		TypeInfo info = reflect(var);

		//Export to stream
		jdoc.ExportTo(stream);
	}

	void binary::deserialize(Var var, std::istream& stream) {
		//Parse Jaguar document
		std::unique_ptr<std::istream> ptr(&stream);
		libjaguar::Document jdoc(std::move(ptr));
		jdoc.MaterializeAll();
		jdoc.ReleaseStream();

		//Business logic...
		TypeInfo info = reflect(var);
	}

}