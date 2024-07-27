#pragma once
#include "common.h"

namespace nifo::components {

	struct selected {};

	struct visitor  {};

	struct disabled {};

	struct hidden {};

	struct root {};

	struct skybox {};

	struct name {
		std::string value;
	};
}