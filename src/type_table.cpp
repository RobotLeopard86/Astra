#include "astra/type_table.hpp"

namespace astra {
	const std::vector<Actions>& TypeTable::actions() {
		return actionsGuard();
	}

	std::size_t TypeTable::recordActions(Actions actions) {
		auto& data = actionsGuard();
		data.push_back(actions);
		return data.size() - 1;
	}

	const std::unordered_map<unsigned int, unsigned int>& TypeTable::submap() {
		return submapGuard();
	}

	void TypeTable::recordMapping(unsigned int original, unsigned int substitute) {
		auto& data = submapGuard();
		data[original] = substitute;
	}

	std::vector<Actions>& TypeTable::actionsGuard() {
		//Zero index for unknown types
		static std::vector<Actions> data = {Actions(&UnknownActions::reflect,
			&UnknownActions::typeName,
			&UnknownActions::sizeOf,
			&UnknownActions::construct,
			&UnknownActions::destroy,
			&UnknownActions::copy,
			&UnknownActions::move)};

		return data;
	}

	std::unordered_map<unsigned int, unsigned int>& TypeTable::submapGuard() {
		//Unknown type maps to unknown type
		static std::unordered_map<unsigned int, unsigned int> data = {{0, 0}};
		return data;
	}
}