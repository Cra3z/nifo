#include <nifo/core/builtin/skybox.h>

namespace nifo::builtin {
	// skybox::skybox(const std::filesystem::path& pos_x, const std::filesystem::path& neg_x,
	// 	const std::filesystem::path& pos_y, const std::filesystem::path& neg_y,
	// 	const std::filesystem::path& pos_z, const std::filesystem::path& neg_z) : mesh(
	// 		mesh::single_buffer_policy_layout{
	// 			.positions = {
	// 			   {-1.f,  1.f,  1.f},
	// 			   {-1.f, -1.f,  1.f},
	// 			   { 1.f, -1.f,  1.f},
	// 			   { 1.f,  1.f,  1.f},
	// 			   { 1.f,  1.f, -1.f},
	// 			   { 1.f, -1.f, -1.f},
	// 			   {-1.f, -1.f, -1.f},
	// 			   {-1.f,  1.f, -1.f}
	// 			}
	// 		},
	// 		{
	// 			0, 1, 3, 3, 1, 2,
	// 			3, 2, 4, 4, 2, 5,
	// 			4, 5, 7, 7, 5, 6,
	// 			7, 6, 0, 0, 6, 1,
	// 			7, 0, 4, 4, 0, 3,
	// 			1, 6, 2, 2, 6, 5
	// 		},
	// 		6,
	// 		material::pool.at("default-skybox")
	// 	),
	// 	map(create_cubemap_from_paths(pos_x, neg_x, pos_y, neg_y, pos_z, neg_z))
	// {}
}
