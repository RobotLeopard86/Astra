#pragma once

#include <string>

namespace templates {
	const std::string Enum =
#include "templates/v0/enum.inc"
		;
	const std::string Header =
#include "templates/v0/header.inc"
		;
	const std::string Object =
#include "templates/v0/object.inc"
		;

	const std::string Enumv1 =
#include "templates/v1/enum.inc"
		;
	const std::string Headerv1 =
#include "templates/v1/header.inc"
		;
	const std::string Objectv1 =
#include "templates/v1/object.inc"
		;
}