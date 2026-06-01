#pragma once

#include "actions.hpp"
#include "dll.hpp"
#include "types/unknown.hpp"

namespace astra {
	/**
	 * @brief Centralized storage for reflected type actions
	 */
	struct ASTRA_API ActionsTable {
		/**
		 * @brief Access the stored data
		 *
		 * @return The stored data array
		 */
		static const inline std::vector<Actions>& data() {
			return dataGuard();
		}

		/**
		 * @brief Record function entries for a new type in the table
		 *
		 * @warning This function should only be called by generated reflection code headers!
		 *
		 * @return The new type's ID number
		 */
		static std::size_t record(Actions actions) {
			auto& data = dataGuard();

			data.push_back(actions);
			return data.size() - 1;
		}

	  private:
		//Static initialization fiasco guard to guarantee the first place for UnknownActions
		static std::vector<Actions>& dataGuard() {
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
	};

}
