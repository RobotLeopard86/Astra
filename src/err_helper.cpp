#include "astra/type_info/variants/sequence/err_helper.hpp"

#include "astra/error/error.hpp"
#include "astra/reflection/reflection.hpp"
#include "astra/tools/format.hpp"
#include "astra/types/all_types.hpp"

namespace astra {
	using namespace astra::sequence;

	Error ErrHelper::error(std::string_view message, TypeId type, TypeId nestedType) {
		return Error(astra::format(message,//
			typeName(type),				   //
			typeName(nestedType)));
	}
}