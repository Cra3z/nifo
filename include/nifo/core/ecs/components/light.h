#pragma once
#include <glm/glm.hpp>
#include "common.h"

namespace nifo::components {
	struct parallel_light {
		glm::vec4    color = glm::vec4{1.f};
		float_t      diffuse = 1.f;
		float_t      specular = 1.f;
	};

	struct point_light {
		glm::vec4    color = glm::vec4{1.f};
		float_t      power = 10.f;
		float_t      diffuse = 1.f;
		float_t      specular = 1.f;
	};

	struct spot_light {
		glm::vec4    color = glm::vec4{1.f};
		float_t      diffuse = 1.f;
		float_t      specular = 1.f;
		float_t      inner_div_outer = 0.85f;
		float_t      outer_cutoff = 60.f;
	};
}