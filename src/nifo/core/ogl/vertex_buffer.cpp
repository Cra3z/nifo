#include <map>
#include <nifo/core/ogl/vertex_buffer.h>

namespace nifo {
	static const std::map<data_type, std::pair<GLenum, GLenum>> attribute_type_table {
				{  data_type::Int, {1, GL_INT}},
				{  data_type::Uint, {1, GL_UNSIGNED_INT}},
				{ data_type::Float, {1, GL_FLOAT}},
				{data_type::Double, {1, GL_DOUBLE}},

				{ data_type::Ivec2, {2, GL_INT}},
				{ data_type::Uvec2, {2, GL_UNSIGNED_INT}},
				{  data_type::Vec2, {2, GL_FLOAT}},
				{ data_type::Dvec2, {2, GL_DOUBLE}},

				{ data_type::Ivec3, {3, GL_INT}},
				{ data_type::Uvec3, {3, GL_UNSIGNED_INT}},
				{  data_type::Vec3, {3, GL_FLOAT}},
				{ data_type::Dvec3, {3, GL_DOUBLE}},

				{  data_type::Ivec4, {4, GL_INT}},
				{  data_type::Uvec4, {4, GL_UNSIGNED_INT}},
				{   data_type::Vec4, {4, GL_FLOAT}},
				{  data_type::Dvec4, {4, GL_DOUBLE}}
	};

	vertex_array::vertex_array() noexcept {
		nifo_gl_invoke(glCreateVertexArrays, 1, &id);
	}

	vertex_array::vertex_array(std::vector<vertex_attribute_format> format) : vertex_array() {
		format_ = std::move(format);
		for (uint_t index = 0; const auto& [attribute_type, stride, relative_offset, normalized] : format_) {
			auto& [size, type] = attribute_type_table.at(attribute_type);
			nifo_gl_invoke(glEnableVertexArrayAttrib, id, index);
			nifo_gl_invoke(glVertexArrayAttribBinding, id, index, index);
			nifo_gl_invoke(glVertexArrayAttribFormat, id, index, size, type, normalized, relative_offset);
			++index;
		}
	}

	vertex_array::vertex_array(const vertex_array& other) : vertex_array() {
		format_ = other.format_;
	}

	vertex_array::vertex_array(vertex_array&& other) noexcept : id(std::exchange(other.id, {})), format_(std::move(other.format_)) {}
	
	vertex_array::~vertex_array() {
		if (id) nifo_gl_invoke(glDeleteVertexArrays, 1, &id);
	}

	auto vertex_array::operator= (const vertex_array& other) -> vertex_array& {
		vertex_array{other}.swap(*this);
		return *this;
	}

	auto vertex_array::operator= (vertex_array&& other) noexcept ->vertex_array& {
		vertex_array{std::move(other)}.swap(*this);
		return *this;
	}

	auto vertex_array::swap(nifo::vertex_array& other) noexcept ->void {
		std::ranges::swap(id, other.id);
		std::ranges::swap(format_, other.format_);
	}

	auto vertex_array::bind() const ->void {
		nifo_gl_invoke(glBindVertexArray, id);
	}

	auto vertex_array::unbind() const -> void {
		nifo_gl_invoke(glBindVertexArray, 0);
	}

	auto vertex_array::set_element_buffer(const element_buffer& ebo) const -> void {
		nifo_gl_invoke(glVertexArrayElementBuffer, id, ebo.id);
	}

	auto vertex_array::set_attribute_buffer(uint_t index, const vertex_buffer& vbo, uint_t offset) const -> void {
		nifo_gl_invoke(glVertexArrayVertexBuffer, id, index, vbo.id, offset, format_.at(index).stride);
	}

	auto vertex_array::format() const noexcept -> const std::vector<vertex_attribute_format>& {
		return format_;
	}

	auto vertex_array::get_id() const noexcept -> uint_t {
		return id;
	}

	auto draw_arrays(primitive mode, int_t first, ssize_t count) ->void {
		nifo_gl_invoke(glDrawArrays, to_underlying(mode), first, count);
	}
	
	auto draw_elements(primitive mode, ssize_t count, const void *indices) ->void {
		nifo_gl_invoke(glDrawElements, to_underlying(mode), count, GL_UNSIGNED_INT, indices);
	}
}