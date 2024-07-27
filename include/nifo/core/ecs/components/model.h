#pragma once
#include "common.h"
#include "../../ogl/model.h"

namespace nifo::components {

	struct model {
		std::shared_ptr<nifo::model> value;
	};

}