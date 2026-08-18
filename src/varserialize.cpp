#include "astra/binary.hpp"
#include "astra/json.hpp"
#include "astra/setup.hpp"
#include "astra/yaml.hpp"
#include "astra/type_table.hpp"
#include "astra/box.hpp"
#include "astra/reflection.hpp"

namespace astra {
	Var resolveInheritance(Var in) {
		//Check if the real type ID is the same as what's reported
		TypeId realType = static_cast<const AstraReflectBase*>(in.raw())->ASTRA__gettypeid();
		if(realType != in.typeId()) {
			return Var(in.isConst() ? const_cast<void*>(in.raw()) : in.rawMut(), realType, in.isConst());
		} else {
			return in;
		}
	}

	void binary::fromDocumentIntoVar(libjaguar::Document& doc, Var var) {
		Var trueVar = resolveInheritance(var);
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(trueVar.typeId().number())));
			deserialize(sub.var(), doc);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(trueVar.rawMut());
		} else {
			deserialize(var, doc);
		}
	}

	void binary::fromVectorIntoVar(const std::vector<uint8_t>& vector, Var var) {
		Var trueVar = resolveInheritance(var);
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(trueVar.typeId().number())));
			deserialize(sub.var(), vector);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(trueVar.rawMut());
		} else {
			deserialize(var, vector);
		}
	}

	void binary::fromStreamIntoVar(std::istream& stream, Var var) {
		Var trueVar = resolveInheritance(var);
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(trueVar.typeId().number())));
			deserialize(sub.var(), stream);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(trueVar.rawMut());
		} else {
			deserialize(var, stream);
		}
	}

	void json::fromJsonIntoVar(const nlohmann::json& json, Var var) {
		Var trueVar = resolveInheritance(var);
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(trueVar.typeId().number())));
			deserialize(sub.var(), json);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(trueVar.rawMut());
		} else {
			deserialize(var, json);
		}
	}

	void yaml::fromNodeIntoVar(const YAML::Node& node, Var var) {
		Var trueVar = resolveInheritance(var);
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(trueVar.typeId().number())));
			deserialize(sub.var(), node);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(trueVar.rawMut());
		} else {
			deserialize(var, node);
		}
	}

	libjaguar::Document binary::toDocumentFromVar(Var var) {
		Var trueVar = resolveInheritance(var);
		libjaguar::Document doc;
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(trueVar.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(trueVar.rawMut());
			serialize(doc, sub.var());
		} else {
			serialize(doc, var);
		}
		return doc;
	}

	std::vector<uint8_t> binary::toVectorFromVar(Var var) {
		Var trueVar = resolveInheritance(var);
		std::vector<uint8_t> result;
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(trueVar.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(trueVar.rawMut());
			serialize(result, sub.var());
		} else {
			serialize(result, var);
		}
		return result;
	}

	nlohmann::json json::toJsonFromVar(Var var) {
		Var trueVar = resolveInheritance(var);
		nlohmann::json json;
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(trueVar.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(trueVar.rawMut());
			serialize(json, sub.var());
		} else {
			serialize(json, var);
		}
		return json;
	}

	YAML::Node yaml::toNodeFromVar(Var var) {
		Var trueVar = resolveInheritance(var);
		YAML::Node node;
		node["__astraforcemapcreate__"] = true;
		node.remove("__astraforcemapcreate__");
		if(TypeTable::submap().contains(trueVar.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(trueVar.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(trueVar.rawMut());
			serialize(node, sub.var());
		} else {
			serialize(node, var);
		}
		return node;
	}
}