#include "astra/binary.hpp"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

#include "astra/box.hpp"
#include "astra/reflection.hpp"
#include "astra/type_info.hpp"
#include "astra/types/array/array.hpp"
#include "astra/types/float/float.hpp"
#include "astra/types/pointer/pointer.hpp"
#include "astra/types/list/list.hpp"
#include "astra/type_actions/all_types.hpp"// IWYU pragma: keep
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
			case TypeInfo::Kind::Float: {
				Float floatInfo = info.asUnsafe<Float>();
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
					case TypeInfo::Kind::Float:
						if(nestedInfo.asUnsafe<Float>().size() == sizeof(float)) {
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
					case TypeInfo::Kind::Float:
						if(nestedInfo.asUnsafe<Float>().size() == sizeof(float)) {
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
				const libjaguar::ValueEntry& ve = doc.QueryValueInfo(path);
				if(auto tagByte = static_cast<uint8_t>(ve.type); tagByte < 0x1A || tagByte > 0x2D) throw std::runtime_error("Found a non-integer value where an integer was expected!");
				if(intInfo.isSigned()) {
					int64_t min, max;
					switch(intInfo.size()) {
						case 8:
							min = INT8_MIN;
							max = INT8_MAX;
							break;
						case 16:
							min = INT16_MIN;
							max = INT16_MAX;
							break;
						case 32:
							min = INT32_MIN;
							max = INT32_MAX;
							break;
						case 64:
							min = INT64_MIN;
							max = INT64_MAX;
							break;
					}
					if(static_cast<uint8_t>(ve.type) >= 0x2A) {
						uint64_t value;
						switch(ve.type) {
							case libjaguar::TypeTag::UInt8: value = doc.QueryValue<uint8_t>(path); break;
							case libjaguar::TypeTag::UInt16: value = doc.QueryValue<uint16_t>(path); break;
							case libjaguar::TypeTag::UInt32: value = doc.QueryValue<uint32_t>(path); break;
							case libjaguar::TypeTag::UInt64: value = doc.QueryValue<uint64_t>(path); break;
							default: throw std::runtime_error("UNREACHABLE CODE");
						}
						if(value > static_cast<uint64_t>(max)) throw std::runtime_error("Integer type coercion failed; value is out of range!");
						intInfo.setSigned(static_cast<int64_t>(value));
					} else {
						int64_t value;
						switch(ve.type) {
							case libjaguar::TypeTag::SInt8: value = doc.QueryValue<int8_t>(path); break;
							case libjaguar::TypeTag::SInt16: value = doc.QueryValue<int16_t>(path); break;
							case libjaguar::TypeTag::SInt32: value = doc.QueryValue<int32_t>(path); break;
							case libjaguar::TypeTag::SInt64: value = doc.QueryValue<int64_t>(path); break;
							default: throw std::runtime_error("UNREACHABLE CODE");
						}
						if(value < min || value > max) throw std::runtime_error("Integer type coercion failed; value is out of range!");
						intInfo.setSigned(value);
					}
				} else {
					uint64_t max;
					switch(intInfo.size()) {
						case 8:
							max = UINT8_MAX;
							break;
						case 16:
							max = UINT16_MAX;
							break;
						case 32:
							max = UINT32_MAX;
							break;
						case 64:
							max = UINT64_MAX;
							break;
					}
					if(static_cast<uint8_t>(ve.type) >= 0x2A) {
						uint64_t value;
						switch(ve.type) {
							case libjaguar::TypeTag::UInt8: value = doc.QueryValue<uint8_t>(path); break;
							case libjaguar::TypeTag::UInt16: value = doc.QueryValue<uint16_t>(path); break;
							case libjaguar::TypeTag::UInt32: value = doc.QueryValue<uint32_t>(path); break;
							case libjaguar::TypeTag::UInt64: value = doc.QueryValue<uint64_t>(path); break;
							default: throw std::runtime_error("UNREACHABLE CODE");
						}
						if(value > max) throw std::runtime_error("Integer type coercion failed; value is out of range!");
						intInfo.setUnsigned(value);
					} else {
						int64_t value;
						switch(ve.type) {
							case libjaguar::TypeTag::SInt8: value = doc.QueryValue<int8_t>(path); break;
							case libjaguar::TypeTag::SInt16: value = doc.QueryValue<int16_t>(path); break;
							case libjaguar::TypeTag::SInt32: value = doc.QueryValue<int32_t>(path); break;
							case libjaguar::TypeTag::SInt64: value = doc.QueryValue<int64_t>(path); break;
							default: throw std::runtime_error("UNREACHABLE CODE");
						}
						if(value < 0 || static_cast<uint64_t>(value) > max) throw std::runtime_error("Integer type coercion failed; value is out of range!");
						intInfo.setUnsigned(static_cast<uint64_t>(value));
					}
				}
				break;
			}
			case TypeInfo::Kind::Float: {
				Float floatInfo = info.asUnsafe<Float>();
				const libjaguar::ValueEntry& ve = doc.QueryValueInfo(path);
				if(ve.type != libjaguar::TypeTag::Float32 && ve.type != libjaguar::TypeTag::Float64) throw std::runtime_error("Found a non-float value where a float was expected!");
				if(ve.type == libjaguar::TypeTag::Float32) {
					floatInfo.set(doc.QueryValue<float>(path));
				} else {
					double value = doc.QueryValue<double>(path);
					if(floatInfo.size() >= sizeof(double)) {
						floatInfo.set(value);
					} else {
						if(std::abs(value) != std::numeric_limits<float>::infinity() && (value < std::numeric_limits<float>::min() || value > std::numeric_limits<float>::max())) throw std::runtime_error("Float type coercion failed; value is out of range!");
						floatInfo.set(value);
					}
				}
				break;
			}
			case TypeInfo::Kind::String: {
				const libjaguar::ValueEntry& ve = doc.QueryValueInfo(path);
				if(ve.type == libjaguar::TypeTag::Boolean) {
					info.asUnsafe<String>().set(doc.QueryValue<bool>(path) ? "true" : "false");
				} else {
					info.asUnsafe<String>().set(doc.QueryValue<std::string>(path));
				}
				break;
			}
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
					TypeTable::actions()[tgt.typeId().number()].move(tgt.rawMut(), nested.var().rawMut());
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