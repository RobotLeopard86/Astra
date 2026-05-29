#include "astra/binary.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

#include "astra/box.hpp"
#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/type_info/array/array.hpp"
#include "astra/type_info/floating/floating.hpp"
#include "astra/type_info/pointer/pointer.hpp"
#include "astra/type_info/list/list.hpp"
#include "astra/types/all_types.hpp"// IWYU pragma: keep
#include "bytestream.hpp"
#include "libjaguar/Index.hpp"
#include "libjaguar/TypeTags.hpp"

namespace astra {
	void serializeBinaryRecursive(libjaguar::Document& doc, const std::string& path, const TypeInfo& info) {
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
				const Object& obj = info.asUnsafe<Object>();
				if(!path.empty()) doc.CreateValue<libjaguar::UnstructuredObjTag>(path);
				for(const auto& [name, contents] : obj.getFields()) {
					std::string subpath = (path.empty() ? "" : path + ".") + std::string(name);
					serializeBinaryRecursive(doc, subpath, reflect(contents.var()));
				}
				break;
			}
			case TypeInfo::Kind::Map: {
				const Map& m = info.asUnsafe<Map>();
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
				const Array& arr = info.asUnsafe<Array>();
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
						{
							size_t idx = 0;
							arr.forEach([&](Var var) {
								TypeInfo subinfo = reflect(var);
								doc.CreateValue<libjaguar::UnstructuredObjTag>(path + "[" + std::to_string(idx) + "]");
								serializeBinaryRecursive(doc, path + "[" + std::to_string(idx) + "].payload", subinfo);
								++idx;
							});
						}
						return;
					default: return;
				}
				size_t idx = 0;
				arr.forEach([&](Var var) {
					TypeInfo subinfo = reflect(var);
					serializeBinaryRecursive(doc, path + "[" + std::to_string(idx) + "]", subinfo);
					++idx;
				});
				break;
			}
			case TypeInfo::Kind::List: {
				const List& list = info.asUnsafe<List>();
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
						{
							size_t idx = 0;
							list.forEach([&](Var var) {
								TypeInfo subinfo = reflect(var);
								doc.CreateValue<libjaguar::UnstructuredObjTag>(path + "[" + std::to_string(idx) + "]");
								serializeBinaryRecursive(doc, path + "[" + std::to_string(idx) + "].payload", subinfo);
								++idx;
							});
						}
						return;
					default: return;
				}
				size_t idx = 0;
				list.forEach([&](Var var) {
					TypeInfo subinfo = reflect(var);
					serializeBinaryRecursive(doc, path + "[" + std::to_string(idx) + "]", subinfo);
					++idx;
				});
				break;
			}
			case TypeInfo::Kind::Pointer: {
				const Pointer& p = info.asUnsafe<Pointer>();
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

	void deserializeBinaryRecursive(libjaguar::Document& doc, const std::string& path, TypeInfo& info) {
		switch(info.getKind()) {
			case TypeInfo::Kind::Bool:
				info.asUnsafe<Bool>().set(doc.QueryValue<bool>(path));
				break;
			case TypeInfo::Kind::Integer: {
				Integer intInfo = info.asUnsafe<Integer>();
				if(intInfo.isSigned()) {
					switch(intInfo.size()) {
						case sizeof(int8_t):
							intInfo.setSigned(doc.QueryValue<int8_t>(path));
							break;
						case sizeof(int16_t):
							intInfo.setSigned(doc.QueryValue<int16_t>(path));
							break;
						case sizeof(int32_t):
							intInfo.setSigned(doc.QueryValue<int32_t>(path));
							break;
						case sizeof(int64_t):
							intInfo.setSigned(doc.QueryValue<int64_t>(path));
							break;
					}
				} else {
					switch(intInfo.size()) {
						case sizeof(uint8_t):
							intInfo.setUnsigned(doc.QueryValue<uint8_t>(path));
							break;
						case sizeof(uint16_t):
							intInfo.setUnsigned(doc.QueryValue<uint16_t>(path));
							break;
						case sizeof(uint32_t):
							intInfo.setUnsigned(doc.QueryValue<uint32_t>(path));
							break;
						case sizeof(uint64_t):
							intInfo.setUnsigned(doc.QueryValue<uint64_t>(path));
							break;
					}
				}
				break;
			}
			case TypeInfo::Kind::Floating: {
				Floating floatInfo = info.asUnsafe<Floating>();
				if(floatInfo.size() == sizeof(float)) {
					floatInfo.set(doc.QueryValue<float>(path));
				} else {
					floatInfo.set(doc.QueryValue<double>(path));
				}
				break;
			}
			case TypeInfo::Kind::String:
				info.asUnsafe<String>().set(doc.QueryValue<std::string>(path));
				break;
			case TypeInfo::Kind::Enum:
				info.asUnsafe<Enum>().fromString(doc.QueryValue<std::string>(path));
				break;
			case TypeInfo::Kind::Object: {
				Object& obj = info.asUnsafe<Object>();
				const libjaguar::ScopeEntry& scopeInfo = doc.QueryScopeInfo(path);
				if(scopeInfo.list) throw std::runtime_error("Invalid object format!");
				if(!scopeInfo.typeID.empty()) throw std::runtime_error("Invalid object format!");
				for(const libjaguar::ValueEntry& ve : scopeInfo.subvalues) {
					std::string subpath = (path.empty() ? "" : path + ".") + std::string(ve.name);
					FieldInfo objField = obj.getField(ve.name);
					TypeInfo fieldInfo = reflect(objField.var());
					deserializeBinaryRecursive(doc, subpath, fieldInfo);
				}
				for(const libjaguar::ScopeEntry& se : scopeInfo.subscopes) {
					std::string subpath = (path.empty() ? "" : path + ".") + std::string(se.name);
					FieldInfo objField = obj.getField(se.name);
					TypeInfo fieldInfo = reflect(objField.var());
					deserializeBinaryRecursive(doc, subpath, fieldInfo);
				}
				break;
			}
			case TypeInfo::Kind::Array: {
				Array& arr = info.asUnsafe<Array>();
				const libjaguar::ScopeEntry& scopeInfo = doc.QueryScopeInfo(path);
				if(!scopeInfo.list) throw std::runtime_error("Invalid list format!");
				if(!scopeInfo.typeID.empty()) throw std::runtime_error("Invalid list format!");
				TypeInfo nestedInfo = reflect(Var(nullptr, arr.nestedType(), false));
				while(nestedInfo.getKind() == TypeInfo::Kind::Pointer) {
					Pointer& ptr = nestedInfo.asUnsafe<Pointer>();
					try {
						nestedInfo = reflect(ptr.getNested());
					} catch(...) {
						ptr.init();
						nestedInfo = reflect(ptr.var());
					}
				}
				for(std::size_t i = 0; i < scopeInfo.subvalues.size() + scopeInfo.subscopes.size(); ++i) {
					Box nested(arr.nestedType());
					nestedInfo.assign(nested.var());
					if(nestedInfo.getKind() == TypeInfo::Kind::Array || nestedInfo.getKind() == TypeInfo::Kind::Array) {
						deserializeBinaryRecursive(doc, ::astra::format("{}[{}].payload", path, i), nestedInfo);
					} else {
						deserializeBinaryRecursive(doc, ::astra::format("{}[{}]", path, i), nestedInfo);
					}
					if(i >= arr.size()) throw std::runtime_error("Too many items in array!");
					Var tgt = arr.at(i);
					if(tgt.isConst()) throw std::runtime_error("Cannot deserialize into const array!");
					ActionsTable::data()[tgt.type().number()].move(tgt.rawMut(), nested.var().rawMut());
				}
				break;
			}
			case TypeInfo::Kind::List: {
				List& list = info.asUnsafe<List>();
				list.clear();
				const libjaguar::ScopeEntry& scopeInfo = doc.QueryScopeInfo(path);
				if(!scopeInfo.list) throw std::runtime_error("Invalid list format!");
				if(!scopeInfo.typeID.empty()) throw std::runtime_error("Invalid list format!");
				TypeInfo nestedInfo = reflect(Var(nullptr, list.nestedType(), false));
				while(nestedInfo.getKind() == TypeInfo::Kind::Pointer) {
					Pointer& ptr = nestedInfo.asUnsafe<Pointer>();
					try {
						nestedInfo = reflect(ptr.getNested());
					} catch(...) {
						ptr.init();
						nestedInfo = reflect(ptr.var());
					}
				}
				for(std::size_t i = 0; i < scopeInfo.subvalues.size() + scopeInfo.subscopes.size(); ++i) {
					Box nested(list.nestedType());
					nestedInfo.assign(nested.var());
					if(nestedInfo.getKind() == TypeInfo::Kind::Array || nestedInfo.getKind() == TypeInfo::Kind::Array) {
						deserializeBinaryRecursive(doc, ::astra::format("{}[{}].payload", path, i), nestedInfo);
					} else {
						deserializeBinaryRecursive(doc, ::astra::format("{}[{}]", path, i), nestedInfo);
					}
					list.push(nested.var());
				}
				break;
			}
			case TypeInfo::Kind::Map: {
				Map& m = info.asUnsafe<Map>();
				m.clear();
				const libjaguar::ScopeEntry& scopeInfo = doc.QueryScopeInfo(path);
				if(!scopeInfo.list) throw std::runtime_error("Invalid map format!");
				if(!scopeInfo.typeID.empty()) throw std::runtime_error("Invalid object format!");
				if(scopeInfo.listElementType != libjaguar::TypeTag::UnstructuredObj) throw std::runtime_error("Invalid map format!");
				for(std::size_t i = 0; i < scopeInfo.subscopes.size(); ++i) {
					Box key(m.keyType()), val(m.valType());
					TypeInfo keyInfo = reflect(key.var()), valInfo = reflect(val.var());
					deserializeBinaryRecursive(doc, ::astra::format("{}[{}].key", path, i), keyInfo);
					deserializeBinaryRecursive(doc, astra::format("{}[{}].val", path, i), valInfo);
					m.insert(key.var(), val.var());
				}
				break;
			}
			case TypeInfo::Kind::Pointer: {
				Pointer& p = info.asUnsafe<Pointer>();
				try {
					TypeInfo nestedInfo = reflect(p.getNested());
					deserializeBinaryRecursive(doc, path, nestedInfo);
				} catch(...) {
					p.init();
					TypeInfo okInfo = reflect(p.var());
					deserializeBinaryRecursive(doc, path, okInfo);
				}
				break;
			};
		}
	}

	void binary::serialize(libjaguar::Document& doc, Var var) {
		serializeBinaryRecursive(doc, "", reflect(var));
	}

	void binary::deserialize(Var var, libjaguar::Document& doc) {
		TypeInfo info = reflect(var);
		deserializeBinaryRecursive(doc, "", info);
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