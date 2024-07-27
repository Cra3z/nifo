#pragma once
#include "../scene.h"
#include "../ogl/texture.h"

namespace nifo::builtin {
	struct skybox : mesh {
		skybox(
			const std::filesystem::path& pos_x, const std::filesystem::path& neg_x,
			const std::filesystem::path& pos_y, const std::filesystem::path& neg_y,
			const std::filesystem::path& pos_z, const std::filesystem::path& neg_z
		);
		cube_map map;
	};
}