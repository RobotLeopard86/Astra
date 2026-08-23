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
		static const std::vector<Actions>& actions();

		/**
		 * @brief Record actions entries for a new type in the table
		 *
		 * @warning This function should only be called by generated reflection code headers!
		 *
		 * @return The new type's ID number
		 */
		static std::size_t recordActions(Actions actions);

		/**
		 * @brief Access the stored substitute type mapping data
		 *
		 * @return The stored mapping
		 */
		static const std::unordered_map<unsigned int, unsigned int>& submap();

		/**
		 * @brief Record entries for the substitute mapper
		 *
		 * @warning This function should only be called by generated reflection code headers!
		 */
		static void recordMapping(unsigned int original, unsigned int substitute);

	  private:
		//Static initialization fiasco guard to guarantee the first place for UnknownActions
		static std::vector<Actions>& actionsGuard();

		//Static initialization fiasco guard to guarantee the first mapping for unknown actions
		static std::unordered_map<unsigned int, unsigned int>& submapGuard();
	};

}
