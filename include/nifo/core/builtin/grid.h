#pragma once
#include "../ogl/mesh.h"

namespace nifo::builtin {
	class grid : public mesh {
	public:
		explicit grid(const nifo::material& material, float side_length = 4.f, unsigned row_count = 100, unsigned column_count = 100);

		[[nodiscard]]
		auto side_length() const noexcept ->float;

		[[nodiscard]]
		auto shape() const noexcept ->glm::uvec2;
	private:
		float side_length_;
		glm::uvec2 shape_;
	};
}