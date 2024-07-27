#include <nifo/core/builtin/rectangle.h>

namespace nifo::builtin {
	rectangle::rectangle(float width, float height, const nifo::material& material) : mesh(
		single_buffer_policy_layout{
			.positions = {},
			.uvs = {}
		},
		{0, 1, 2, 2, 3, 0},
		1,
		material
	) {

	}
}
