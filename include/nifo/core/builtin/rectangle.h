#pragma once
#include "../ogl/mesh.h"

namespace nifo::builtin {
	struct rectangle : mesh {
		rectangle(float width, float height, const nifo::material& material);
	};
}