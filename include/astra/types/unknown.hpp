#pragma once

#include <cstddef>

#include "astra/type_info.hpp"
#include "astra/dll.hpp"

namespace astra {

	struct ASTRA_API UnknownActions {
		static TypeInfo reflect(void* /*value*/, bool /*isConst*/) {
			throw std::runtime_error("Cannot reflect a value with unknown type");
		}

		static const std::string& typeName() {
			static std::string name = "unknowntype";
			return name;
		}

		static std::size_t sizeOf() {
			return 0;
		}

		static void construct(void* /*p*/) {
			throw std::runtime_error("Cannot construct ASTRA_API a value for unknown type");
		}

		static void destroy(void* /*p*/) {
			throw std::runtime_error("Cannot destroy a value with unknown type");
		}

		static void copy(void* /*to*/, const void* /*from*/) {
			throw std::runtime_error("Cannot copy a value with unknown type");
		}

		static void move(void* /*to*/, void* /*from*/) {
			throw std::runtime_error("Cannot move a value with unknown type");
		}
	};

}
