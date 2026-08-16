#include "astra/binary.hpp"
#include "astra/json.hpp"
#include "astra/yaml.hpp"
#include "astra/type_table.hpp"
#include "astra/box.hpp"
#include "astra/reflection.hpp"

namespace astra {
	void binary::fromDocumentIntoVar(libjaguar::Document& doc, Var var) {
		if(TypeTable::submap().contains(var.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(var.typeId().number())));
			deserialize(sub.var(), doc);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(var.rawMut());
		} else {
			deserialize(var, doc);
		}
	}

	void binary::fromVectorIntoVar(const std::vector<uint8_t>& vector, Var var) {
		if(TypeTable::submap().contains(var.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(var.typeId().number())));
			deserialize(sub.var(), vector);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(var.rawMut());
		} else {
			deserialize(var, vector);
		}
	}

	void binary::fromStreamIntoVar(std::istream& stream, Var var) {
		if(TypeTable::submap().contains(var.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(var.typeId().number())));
			deserialize(sub.var(), stream);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(var.rawMut());
		} else {
			deserialize(var, stream);
		}
	}

	void json::fromJsonIntoVar(const nlohmann::json& json, Var var) {
		if(TypeTable::submap().contains(var.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(var.typeId().number())));
			deserialize(sub.var(), json);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(var.rawMut());
		} else {
			deserialize(var, json);
		}
	}

	void yaml::fromNodeIntoVar(const YAML::Node& node, Var var) {
		if(TypeTable::submap().contains(var.typeId().number())) {
			Box sub(TypeId(TypeTable::submap().at(var.typeId().number())));
			deserialize(sub.var(), node);
			reflect(sub.var()).as<Object>().getMethod("ASTRA__deserializeinternal").invoke<void, void*>(var.rawMut());
		} else {
			deserialize(var, node);
		}
	}

	libjaguar::Document binary::toDocumentFromVar(Var var) {
		libjaguar::Document doc;
		if(TypeTable::submap().contains(var.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(var.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(var.rawMut());
			serialize(doc, sub.var());
		} else {
			serialize(doc, var);
		}
		return doc;
	}

	std::vector<uint8_t> binary::toVectorFromVar(Var var) {
		std::vector<uint8_t> result;
		if(TypeTable::submap().contains(var.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(var.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(var.rawMut());
			serialize(result, sub.var());
		} else {
			serialize(result, var);
		}
		return result;
	}

	nlohmann::json json::toJsonFromVar(Var var) {
		nlohmann::json json;
		if(TypeTable::submap().contains(var.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(var.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(var.rawMut());
			serialize(json, sub.var());
		} else {
			serialize(json, var);
		}
		return json;
	}

	YAML::Node yaml::toNodeFromVar(Var var) {
		YAML::Node node;
		node["__astraforcemapcreate__"] = true;
		node.remove("__astraforcemapcreate__");
		if(TypeTable::submap().contains(var.typeId().number())) {
			uint32_t subId = TypeTable::submap().at(var.typeId().number());
			Box sub(TypeId {subId});
			reflect(sub.var()).as<Object>().getMethod("ASTRA__serializeinternal").invoke<void, void*>(var.rawMut());
			serialize(node, sub.var());
		} else {
			serialize(node, var);
		}
		return node;
	}
}