#include "astra/binary.hpp"

#include <iostream>
#include <memory>

#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/type_info/array/array.hpp"
#include "astra/type_info/floating/floating.hpp"
#include "astra/type_info/pointer/pointer.hpp"
#include "astra/type_info/list/list.hpp"
#include "astra/types/all_types.hpp"// IWYU pragma: keep
#include "bytestream.hpp"

namespace astra {
	/*inline void deserializeBinaryRecursive(TypeInfo* info, const GroupReader& reader) {
		auto k = info->getKind();

		switch(k) {
			case TypeInfo::Kind::kObject:
				for(auto&& record : info->asUnsafe<Object>().getFields()) {
					//skip name in record.first;
					auto fieldInfo = reflect(record.second.var());
					deserializeBinaryRecursive(&fieldInfo, reader);
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
			break; }
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

					deserializeBinaryRecursive(&keyInfo, reader);

					Box valBox(m.valType());
					valInfo.unsafeAssign(valBox.var().rawMut());

					deserializeBinaryRecursive(&valInfo, reader);

					m.insert(keyBox.var(), valBox.var());
				}
			break; }
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
					deserializeBinaryRecursive(&entryInfo, reader);
				});
			break; }
			case TypeInfo::Kind::kList: {
				auto s = info->asUnsafe<List>();
				s.clear();

				auto entryInfo = reflect(Var(nullptr, s.nestedType(), false));

				auto n = reader.readUnsigned();
				for(auto i = 0; i < n; i++) {
					Box entryBox(s.nestedType());//Box should be a new object for each iteration
					entryInfo.unsafeAssign(entryBox.var().rawMut());

					deserializeBinaryRecursive(&entryInfo, reader);

					s.push(entryBox.var());
				}
			break; }
			case TypeInfo::Kind::kPointer: {
				if(reader.isNull()) {
					reader.readUnsigned();//skip a byte
					return;
				}
				auto p = info->asUnsafe<Pointer>();
				try {
					Var var = p.getNested();
					TypeInfo nestedInfo = reflect(var);
					deserializeBinaryRecursive(&nestedInfo, reader);
				} catch(...) {
					p.init();
					TypeInfo nestedInfo = reflect(p.var());
					deserializeBinaryRecursive(&nestedInfo, reader);
				}
			break; }
		}
	}*/

	static void serializeBinaryRecursive(libjaguar::Document& doc, const std::string& path, const TypeInfo& info) {
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool: {
				bool val = info.asUnsafe<Bool>().get();
				doc.SetOrCreateValue(path, val);
				break;
			}
			case TypeInfo::Kind::Integer: {
				Integer intInfo = info.asUnsafe<Integer>();
				if(intInfo.isSigned()) {
					switch(intInfo.size()) {
						case sizeof(int8_t):
							doc.SetOrCreateValue<int8_t>(path, (int8_t)intInfo.asSigned());
							break;
						case sizeof(int16_t):
							doc.SetOrCreateValue<int16_t>(path, (int16_t)intInfo.asSigned());
							break;
						case sizeof(int32_t):
							doc.SetOrCreateValue<int32_t>(path, (int32_t)intInfo.asSigned());
							break;
						case sizeof(int64_t):
							doc.SetOrCreateValue<int64_t>(path, intInfo.asSigned());
							break;
					}
				} else {
					switch(intInfo.size()) {
						case sizeof(uint8_t):
							doc.SetOrCreateValue<uint8_t>(path, (uint8_t)intInfo.asUnsigned());
							break;
						case sizeof(uint16_t):
							doc.SetOrCreateValue<uint16_t>(path, (uint16_t)intInfo.asUnsigned());
							break;
						case sizeof(uint32_t):
							doc.SetOrCreateValue<uint32_t>(path, (uint32_t)intInfo.asUnsigned());
							break;
						case sizeof(uint64_t):
							doc.SetOrCreateValue<uint64_t>(path, intInfo.asUnsigned());
							break;
					}
				}
				break;
			}
			case TypeInfo::Kind::Floating: {
				Floating floatInfo = info.asUnsafe<Floating>();
				if(floatInfo.size() == sizeof(float)) {
					doc.SetOrCreateValue<float>(path, (float)floatInfo.get());
				} else {
					doc.SetOrCreateValue<double>(path, floatInfo.get());
				}
				break;
			}
			case TypeInfo::Kind::String: {
				std::string val = info.asUnsafe<String>().get();
				doc.SetOrCreateValue(path, val);
				break;
			}
			case TypeInfo::Kind::Enum: {
				std::string val = info.asUnsafe<Enum>().toString();
				doc.SetOrCreateValue(path, val);
				break;
			}
			case TypeInfo::Kind::Object: {
				const auto& obj = info.asUnsafe<Object>();
				doc.CreateValue<libjaguar::UnstructuredObjTag>(path);
				for(const auto& [name, contents] : obj.getFields()) {
					std::string subpath = path + "." + std::string(name);
					serializeBinaryRecursive(doc, subpath, reflect(contents.var()));
				}
				break;
			}
			case TypeInfo::Kind::Map: {
				const auto& m = info.asUnsafe<Map>();
				size_t idx = 0;
				doc.CreateValue<std::vector<libjaguar::UnstructuredObjTag>>(path);
				m.forEach([&](Var key, Var val) {
					std::string subpath = path + "[" + std::to_string(idx++) + "]";
					doc.CreateValue<libjaguar::UnstructuredObjTag>(subpath);
					serializeBinaryRecursive(doc, subpath + ".key", reflect(key));
					serializeBinaryRecursive(doc, subpath + ".val", reflect(val));
				});
				break;
			}
			case TypeInfo::Kind::Array: {
				const auto& arr = info.asUnsafe<Array>();
				TypeInfo nestedInfo = reflect(Var(nullptr, arr.nestedType(), false));
				while(nestedInfo.getKind() == TypeInfo::Kind::Pointer) {
					try {
						nestedInfo = reflect(nestedInfo.asUnsafe<Pointer>().getNested());
					} catch(...) {
						return;
					}
				}
				switch(nestedInfo.getKind()) {
					case TypeInfo::Kind::Bool:
						doc.CreateValue<std::vector<bool>>(path);
						break;
					case TypeInfo::Kind::Integer:
						if(auto intInfo = nestedInfo.asUnsafe<Integer>(); intInfo.isSigned()) {
							switch(intInfo.size()) {
								case sizeof(int8_t):
									doc.CreateValue<std::vector<int8_t>>(path);
									break;
								case sizeof(int16_t):
									doc.CreateValue<std::vector<int16_t>>(path);
									break;
								case sizeof(int32_t):
									doc.CreateValue<std::vector<int32_t>>(path);
									break;
								case sizeof(int64_t):
									doc.CreateValue<std::vector<int64_t>>(path);
									break;
							}
						} else {
							switch(intInfo.size()) {
								case sizeof(uint8_t):
									doc.CreateValue<std::vector<uint8_t>>(path, true);
									break;
								case sizeof(uint16_t):
									doc.CreateValue<std::vector<uint16_t>>(path);
									break;
								case sizeof(uint32_t):
									doc.CreateValue<std::vector<uint32_t>>(path);
									break;
								case sizeof(uint64_t):
									doc.CreateValue<std::vector<uint64_t>>(path);
									break;
							}
						}
						break;
					case TypeInfo::Kind::Floating:
						if(nestedInfo.asUnsafe<Floating>().size() == sizeof(float)) {
							doc.CreateValue<std::vector<float>>(path);
						} else {
							doc.CreateValue<std::vector<double>>(path);
						}
						break;
					case TypeInfo::Kind::String:
					case TypeInfo::Kind::Enum:
						doc.CreateValue<std::vector<std::string>>(path);
						break;
					case TypeInfo::Kind::Array:
					case TypeInfo::Kind::List:
					case TypeInfo::Kind::Map:
					case TypeInfo::Kind::Object:
						doc.CreateValue<std::vector<libjaguar::UnstructuredObjTag>>(path);
						if(nestedInfo.getKind() == TypeInfo::Kind::Object || nestedInfo.getKind() == TypeInfo::Kind::Map) break;
						serializeBinaryRecursive(doc, path + ".payload", nestedInfo);
						break;
					default: return;
				}
				size_t idx = 0;
				arr.unsafeForEach([&](void* ptr) {
					TypeInfo nestedInfo = reflect(Var(nullptr, arr.nestedType(), false));
					nestedInfo.unsafeAssign(ptr);
					serializeBinaryRecursive(doc, path + "[" + std::to_string(idx) + "]", nestedInfo);
					++idx;
				});
				break;
			}
			case TypeInfo::Kind::List: {
				const auto& list = info.asUnsafe<List>();
				TypeInfo nestedInfo = reflect(Var(nullptr, list.nestedType(), false));
				while(nestedInfo.getKind() == TypeInfo::Kind::Pointer) {
					try {
						nestedInfo = reflect(nestedInfo.asUnsafe<Pointer>().getNested());
					} catch(...) {
						return;
					}
				}
				switch(nestedInfo.getKind()) {
					case TypeInfo::Kind::Bool:
						doc.CreateValue<std::vector<bool>>(path);
						break;
					case TypeInfo::Kind::Integer:
						if(auto intInfo = nestedInfo.asUnsafe<Integer>(); intInfo.isSigned()) {
							switch(intInfo.size()) {
								case sizeof(int8_t):
									doc.CreateValue<std::vector<int8_t>>(path);
									break;
								case sizeof(int16_t):
									doc.CreateValue<std::vector<int16_t>>(path);
									break;
								case sizeof(int32_t):
									doc.CreateValue<std::vector<int32_t>>(path);
									break;
								case sizeof(int64_t):
									doc.CreateValue<std::vector<int64_t>>(path);
									break;
							}
						} else {
							switch(intInfo.size()) {
								case sizeof(uint8_t):
									doc.CreateValue<std::vector<uint8_t>>(path, true);
									break;
								case sizeof(uint16_t):
									doc.CreateValue<std::vector<uint16_t>>(path);
									break;
								case sizeof(uint32_t):
									doc.CreateValue<std::vector<uint32_t>>(path);
									break;
								case sizeof(uint64_t):
									doc.CreateValue<std::vector<uint64_t>>(path);
									break;
							}
						}
						break;
					case TypeInfo::Kind::Floating:
						if(nestedInfo.asUnsafe<Floating>().size() == sizeof(float)) {
							doc.CreateValue<std::vector<float>>(path);
						} else {
							doc.CreateValue<std::vector<double>>(path);
						}
						break;
					case TypeInfo::Kind::String:
					case TypeInfo::Kind::Enum:
						doc.CreateValue<std::vector<std::string>>(path);
						break;
					case TypeInfo::Kind::Array:
					case TypeInfo::Kind::List:
					case TypeInfo::Kind::Map:
					case TypeInfo::Kind::Object:
						doc.CreateValue<std::vector<libjaguar::UnstructuredObjTag>>(path);
						if(nestedInfo.getKind() == TypeInfo::Kind::Object || nestedInfo.getKind() == TypeInfo::Kind::Map) break;
						serializeBinaryRecursive(doc, path + ".payload", nestedInfo);
						break;
					default: return;
				}
				size_t idx = 0;
				list.unsafeForEach([&](void* ptr) {
					TypeInfo nestedInfo = reflect(Var(nullptr, list.nestedType(), false));
					nestedInfo.unsafeAssign(ptr);
					serializeBinaryRecursive(doc, path + "[" + std::to_string(idx) + "]", nestedInfo);
					++idx;
				});
				break;
			}
			case TypeInfo::Kind::Pointer: {
				const auto& p = info.asUnsafe<Pointer>();
				try {
					Var nested = p.getNested();
					serializeBinaryRecursive(doc, path, reflect(nested));
				} catch(...) {
					//Can't serialize nullptr
				}
				break;
			}
		}
	}

	void binary::serialize(libjaguar::Document& doc, Var var) {
		serializeBinaryRecursive(doc, "root", reflect(var));
	}

	void binary::deserialize(Var var, libjaguar::Document& doc) {
		//TODO: business logic
	}

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
		//Forward via Jaguar document
		libjaguar::Document jdoc;
		serialize(jdoc, var);

		//Export to stream
		jdoc.ExportTo(stream);
	}

	void binary::deserialize(Var var, std::istream& stream) {
		//Forward via document
		std::unique_ptr<std::istream> ptr(&stream);
		libjaguar::Document doc(std::move(ptr));
		doc.MaterializeAll();
		doc.ReleaseStream();
		deserialize(var, doc);
	}
}