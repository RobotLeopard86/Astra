#include "astra/type_info/list/err_helper.hpp"


#include "astra/reflection.hpp"
#include "astra/format.hpp"
#include "astra/types/all_types.hpp"

namespace astra {
	using namespace astra::list;

	void ErrHelper::error(const std::string& message, TypeId type, TypeId nestedType) {
		throw std::runtime_error(::astra::format(message,//
			typeName(type),								 //
			typeName(nestedType)));
	}
}