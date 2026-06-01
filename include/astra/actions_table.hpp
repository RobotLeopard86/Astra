#pragma once

#include "actions.hpp"
#include "dll.hpp"
#include "types/unknown.hpp"

namespace astra {
	/**
	 * @brief
	 */
	struct ASTRA_API ActionsTable {
		static const inline std::vector<Actions>& data() {
			return dataGuard();
		}

		static std::size_t record(Actions actions) {
			auto& data = dataGuard();

			data.push_back(actions);
			return data.size() - 1;
		}

	  private:
		//static initialization fiasco guard to guarantee the first place for UnknownActions
		static std::vector<Actions>& dataGuard() {
			//zero index for unknown type
			static std::vector<Actions> data = {Actions(&UnknownActions::reflect,//
				&UnknownActions::typeName,										 //
				&UnknownActions::sizeOf,										 //
				&UnknownActions::construct,										 //
				&UnknownActions::destroy,										 //
				&UnknownActions::copy,											 //
				&UnknownActions::move)};

			return data;
		}
	};

}
