#include <nifo/core/builtin/grid.h>

namespace nifo::builtin {
	grid::grid(const nifo::material& material, float side_length, unsigned row_count, unsigned column_count) : mesh(
		[&] () ->mesh {
			// 其中layout.positions中glm::vec3的y表示线条颜色: 1.f: 普通线条, 2.f: 每10个格子之间的界限, 4.f: x轴, 8.f: z轴
			mesh::single_buffer_policy_layout layout{
				.positions = {
					{-side_length * float(column_count) / 2.f, 4.f, 0.f},
					{ side_length * float(column_count) / 2.f, 4.f, 0.f},
					{ 0.f, 8.f, -side_length * float(row_count) / 2.f},
					{ 0.f, 8.f,  side_length * float(row_count) / 2.f}
				}
			};
			for (const auto side : {-1.f, 1.f}) {
				for (unsigned i = 1; i <= row_count / 2; ++i) {
					const auto color = i % 10 ? 1.f : 2.f;
					layout.positions.emplace_back(-side_length * float(column_count) / 2.f, color, side * side_length * float(i));
					layout.positions.emplace_back( side_length * float(column_count) / 2.f, color, side * side_length * float(i));
				}
				for (unsigned i = 1; i <= column_count / 2; ++i) {
					const auto color = i % 10 ? 1.f : 2.f;
					layout.positions.emplace_back(side * side_length * float(i), color, -side_length * float(row_count) / 2.f);
					layout.positions.emplace_back(side * side_length * float(i), color,  side_length * float(row_count) / 2.f);
				}
			}
			auto iota = std::views::iota(0u, unsigned(layout.positions.size()));
			return {layout, std::vector(iota.begin(), iota.end()), 1, std::ref(material)};
		}()
	), side_length_(side_length), shape_(row_count, column_count) {}

	auto grid::side_length() const noexcept -> float {
		return side_length_;
	}

	auto grid::shape() const noexcept -> glm::uvec2 {
		return shape_;
	}
}
