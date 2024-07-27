#include <nifo/core/ogl/mesh.h>

namespace nifo {

	auto material::pass_attributes_to_shader(texture2d_manager& texture2d_manager) const -> void {
		texture_unit = 0;
		for (const auto& [key, value] : attributes) {
			std::visit(
				[&] (const auto& uniform_value) {
					if constexpr (std::same_as<std::remove_cvref_t<decltype(uniform_value)>, std::filesystem::path>) {
						texture2d_manager.load(uniform_value).bind_unit(texture_unit);
						shader.try_set_uniform("material." + key, texture_unit++);
					}
					else shader.try_set_uniform("material." + key, uniform_value);
				},
				value
			);
		}
		texture_unit = 0;
	}

	mesh::mesh(std::vector<vertex_layout> vertices, std::vector<uint_t> indices, std::size_t face_count, const nifo::material& material) :
		interleaved_buffer_(std::move(vertices)), indices_(std::move(indices)), face_count_(face_count), material_(material),
		vao_{{
			{
				.type = data_type::Vec3,
				.stride = sizeof(vertex_layout)
			},
			{
				.type = data_type::Vec3,
				.stride = sizeof(vertex_layout),
				.relative_offset = sizeof(glm::vec3)
			},
			{
				.type = data_type::Vec2,
				.stride = sizeof(vertex_layout),
				.relative_offset = 2 * sizeof(glm::vec3)
			}
		}}, ebo_(create_element_buffer(indices_)), policy_(buffer_policy::interleaved) {
		vbos_.push_back(create_vertex_buffer(interleaved_buffer_));
		vao_.set_attribute_buffer(0, vbos_.front());
		vao_.set_attribute_buffer(1, vbos_.front());
		vao_.set_attribute_buffer(2, vbos_.front());
		vao_.set_element_buffer(ebo_);
	}

	mesh::mesh(single_buffer_policy_layout buffers, std::vector<uint_t> indices, std::size_t face_count, const nifo::material& material) :
		single_buffers_(std::move(buffers)), indices_(std::move(indices)), face_count_(face_count), material_(material),
		ebo_(create_element_buffer(indices_)),
		policy_(buffer_policy::single)
	{
		std::size_t n = 0;
		std::vector<vertex_attribute_format> attribute_formats;
		if (not single_buffers_.positions.empty()) {
			vbos_.push_back(create_vertex_buffer(single_buffers_.positions));
			attribute_formats.push_back({.type = data_type::Vec3, .stride = sizeof(glm::vec3)});
			++n;
		}
		if (not single_buffers_.normals.empty()) {
			vbos_.push_back(create_vertex_buffer(single_buffers_.normals));
			attribute_formats.push_back({.type = data_type::Vec3, .stride = sizeof(glm::vec3)});
			++n;
		}
		if (not single_buffers_.uvs.empty()) {
			vbos_.push_back(create_vertex_buffer(single_buffers_.uvs));
			attribute_formats.push_back({.type = data_type::Vec2, .stride = sizeof(glm::vec2)});
			++n;
		}
		vao_ = vertex_array{std::move(attribute_formats)};
		for (std::size_t i = 0; i < n; ++i) {
			vao_.set_attribute_buffer(i, vbos_[i]);
		}
		vao_.set_element_buffer(ebo_);
	}

	auto mesh::indices() const noexcept -> const std::vector<uint_t>& {
		return indices_;
	}

	auto mesh::vertex_count() const noexcept -> std::size_t {
		return interleaved_buffer_.size();
	}

	auto mesh::index_count() const noexcept -> std::size_t {
		return indices_.size();
	}

	auto mesh::face_count() const noexcept -> std::size_t {
		return face_count_;
	}

	auto mesh::material() const noexcept ->const nifo::material& {
		return material_;
	}

	auto mesh::draw(const std::function<primitive(const nifo::material&)>& callback) const -> void {
		draw_with(material_, callback);
	}

	auto mesh::draw_with(const nifo::material& material, const std::function<primitive(const nifo::material&)>& callback) const -> void {
		material.shader.use();
		nifo_expect(std::in_range<int>(indices_.size()));
		vao_.bind();
		draw_elements(callback(material), /* NOLINT */ indices_.size());
	}

	auto mesh::vao() const noexcept -> const vertex_array& {
		return vao_;
	}

	auto mesh::vertex_buffer_policy() const noexcept -> buffer_policy {
		return policy_;
	}

	name_generator material::name_generator;
}
