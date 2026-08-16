#pragma once

#include "actions.hpp"
#include "dll.hpp"
#include "type_actions/unknown.hpp"
#include <unordered_map>

namespace astra {
	/**
	 * @brief Centralized storage for reflected type information
	 */
	struct ASTRA_API TypeTable {
		/**
		 * @brief Access the stored actions data
		 *
		 * @return The stored actions data array
		 */
		static const inline std::vector<Actions>& actions() {
			return actionsGuard();
		}

		/**
		 * @brief Record actions entries for a new type in the table
		 *
		 * @warning This function should only be called by generated reflection code headers!
		 *
		 * @return The new type's ID number
		 */
		static std::size_t recordActions(Actions actions) {
			auto& data = actionsGuard();

			data.push_back(actions);
			return data.size() - 1;
		}

		/**
		 * @brief Access the stored substitute type mapping data
		 *
		 * @return The stored mapping
		 */
		static const inline std::unordered_map<unsigned int, unsigned int>& submap() {
			return submapGuard();
		}

		/**
		 * @brief Record entries for the substitute mapper
		 *
		 * @warning This function should only be called by generated reflection code headers!
		 */
		static void recordMapping(unsigned int original, unsigned int substitute) {
			auto& data = submapGuard();
			data[original] = substitute;
		}

	  private:
		//Static initialization fiasco guard to guarantee the first place for UnknownActions
		static std::vector<Actions>& actionsGuard() {
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

		//Static initialization fiasco guard to guarantee the first mapping for unknown actions
		static std::unordered_map<unsigned int, unsigned int>& submapGuard() {
			//Unknown type maps to unknown type
			static std::unordered_map<unsigned int, unsigned int> data = {{0, 0}};
			return data;
		}
	};

}
