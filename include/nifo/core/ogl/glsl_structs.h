#pragma once
#include <glm/glm.hpp>
#include "def.h"
#include "../ecs/components/light.h"

namespace nifo::glsl {

	inline namespace std430 {
		struct alignas(4 * sizeof(float_t)) parallel_light {
			glm::vec3                  direction;
			std::byte                  padding[sizeof(float_t)]{};
			components::parallel_light light_info;
		};

		struct alignas(4 * sizeof(float_t)) point_light {
			glm::vec3                  position;
			std::byte                  padding[sizeof(float_t)]{};
			components::point_light    light_info;
		};

		struct alignas(4 * sizeof(float_t)) spot_light {
			glm::vec3	               position;
			std::byte                  padding_0[sizeof(float_t)]{};
			glm::vec3	               direction;
			std::byte                  padding_1[sizeof(float_t)]{};
			components::spot_light     light_info;
		};
	}

	inline namespace std140 {
		struct camera_uniforms {
			glm::vec3    eye;
			std::byte    padding[sizeof(float_t)];
			glm::mat4    view_projection_matrix;
		};
	}

}