#include <algorithm>
#include <map>
#include <ranges>
#include <regex>
#include <utility>
#include <nifo/core/ogl/shader.h>

namespace {

	std::unordered_map<nifo::uint_t, std::map<std::string, nifo::int_t, std::less<>>> shader_program_uniform_location_cached{};

	std::unordered_map<nifo::uint_t, std::map<std::string, nifo::uint_t, std::less<>>> shader_program_uniform_block_location_cached{};

}

namespace nifo {
	
	shader_part::shader_part(std::filesystem::path file_path) : associated_file_(std::move(file_path)) {
		type_ = recongnize_file_type_(associated_file_);
		if (type_ == shader_type::none) [[unlikely]] throw std::invalid_argument{fmt::format("unrecognized file type: \"{}\"", associated_file_)};
		std::ifstream file{associated_file_};
		if (not file.is_open()) [[unlikely]] throw runtime_error{fmt::format("failed to open the shader file \"{}\"", associated_file_)};
		shader_id = nifo_gl_invoke(glCreateShader, to_underlying(type_));
		std::vector<std::string> source_code{};
		std::string a_line{};
		while (std::getline(file, a_line)) {
			a_line.push_back('\n');
			source_code.emplace_back(std::move(a_line));
			a_line.clear();
		}
		std::vector<const char*> source_code_{};
		std::ranges::transform(source_code, std::back_inserter(source_code_), [](const std::string& str){return str.c_str();});
		nifo_gl_invoke(glShaderSource, shader_id, source_code.size(), source_code_.data(), nullptr);
		nifo_gl_invoke(glCompileShader, shader_id);
		check_shader_error_();
	}
	
	shader_part::shader_part(shader_part&& other) noexcept : associated_file_(std::exchange(other.associated_file_, {})), shader_id(std::exchange(other.shader_id, {})), type_(std::exchange(other.type_, {})) {}
	
	auto shader_part::operator= (shader_part&& other) noexcept ->shader_part& {
		shader_part tmp{std::move(other)};
		swap(tmp);
		return *this;
	}
	
	shader_part::~shader_part() {
		nifo_gl_invoke(glDeleteShader, shader_id);
	}

	auto shader_part::swap(shader_part& other) noexcept ->void {
		std::ranges::swap(associated_file_, other.associated_file_);
		std::ranges::swap(shader_id, other.shader_id);
		std::ranges::swap(type_, other.type_);
	}
	
	auto shader_part::type() const noexcept ->shader_type {
		return type_;
	}

	auto shader_part::associated_file() const noexcept -> const std::filesystem::path& {
		return associated_file_;
	}

	auto shader_part::recongnize_file_type_(const std::filesystem::path& file_path) -> shader_type {
		auto shader_type = file_path.extension().string();
		if (shader_type == ".vert" or shader_type == ".vs") return shader_type::vertex_shader;
		if (shader_type == ".geom" or shader_type == ".gs") return shader_type::geometry_shader;
		if (shader_type == ".frag" or shader_type == ".fs") return shader_type::fragment_shader;
		if (shader_type == ".comp") return shader_type::compute_shader;
		if (shader_type == ".tesc") return shader_type::tess_control_shader;
		if (shader_type == ".tese") return shader_type::tess_evaluation_shader;
		return shader_type::none;
	}

	auto shader_part::check_shader_error_() ->void {
		int_t errc;
		nifo_gl_invoke(glGetShaderiv, shader_id, GL_COMPILE_STATUS, &errc);
		if (!errc) {
			int_t len;
			nifo_gl_invoke(glGetShaderiv, shader_id, GL_INFO_LOG_LENGTH, &len);
			std::vector<char> errors(len);
			nifo_gl_invoke(glGetShaderInfoLog, shader_id, len, &len, errors.data());
			errors.emplace_back('\0');
			nifo_gl_invoke(glDeleteShader, shader_id);
			shader_id = 0;
			type_ = shader_type::none;
			throw runtime_error{fmt::format("[Shader Compilation Error] in file \"{}\"\n{}", associated_file_, std::string_view{errors.begin(), errors.end()})};
		}
	}

	shader_program::shader_program(int) noexcept : program_id(nifo_gl_invoke(glCreateProgram)) {}

	shader_program::shader_program(std::shared_ptr<shader_part> vert_shader, std::shared_ptr<shader_part> frag_shader) : shader_program(0) {
		vert = std::move(vert_shader);
		frag = std::move(frag_shader);
		if (
			vert->type() != shader_type::vertex_shader or
			frag->type() != shader_type::fragment_shader
		) throw std::invalid_argument{"`cgl::shader_program::shader_program(shader& vert_shader, shader& frag_shader)`: the parameter `vert_shader` or `frag_shader` is invalid"};
		nifo_gl_invoke(glAttachShader, program_id, vert->shader_id);
		nifo_gl_invoke(glAttachShader, program_id, frag->shader_id);
		nifo_gl_invoke(glLinkProgram, program_id);
		check_program_link_error_();
		nifo_gl_invoke(glValidateProgram, program_id);
		check_program_valid_();
	}

	shader_program::shader_program(std::shared_ptr<shader_part> vert_shader, std::shared_ptr<shader_part> geom_shader, std::shared_ptr<shader_part> frag_shader) : shader_program(0) {
		vert = std::move(vert_shader);
		geom = std::move(geom_shader);
		frag = std::move(frag_shader);
		if (
			vert->type() != shader_type::vertex_shader or
			geom->type() != shader_type::geometry_shader or
			frag->type() != shader_type::fragment_shader
		) throw std::invalid_argument{"`nifo::shader_program::shader_program(shader& vert_shader, shader& frag_shader)`: the parameter `vert_shader`, `geom_shader` or `frag_shader` is invalid"};
		nifo_gl_invoke(glAttachShader, program_id, vert->shader_id);
		nifo_gl_invoke(glAttachShader, program_id, geom->shader_id);
		nifo_gl_invoke(glAttachShader, program_id, frag->shader_id);
		nifo_gl_invoke(glLinkProgram, program_id);
		check_program_link_error_();
		nifo_gl_invoke(glValidateProgram, program_id);
		check_program_valid_();
	}

	shader_program::shader_program(const shader_program& other) {
		if (other.vert && other.frag) {
			if (other.geom) shader_program{other.vert, other.geom, other.frag}.swap(*this);
			else shader_program{other.vert, other.frag}.swap(*this);
		}
	}

	shader_program::shader_program(shader_program&& other) noexcept : program_id(std::exchange(other.program_id, {})), vert(std::move(other.vert)), geom(std::move(other.geom)), frag(std::move(other.frag)) {}
	
	shader_program::~shader_program() {
		if (program_id) nifo_gl_invoke(glDeleteProgram, program_id);
	}

	auto shader_program::operator= (const shader_program& other) ->shader_program& {
		shader_program{other}.swap(*this);
		return *this;
	}

	auto shader_program::operator= (shader_program&& other) noexcept ->shader_program& {
		shader_program{std::move(other)}.swap(*this);
		return *this;
	}
	
	auto shader_program::swap(shader_program& other) noexcept ->void {
		std::ranges::swap(program_id, other.program_id);
		std::ranges::swap(vert, other.vert);
		std::ranges::swap(geom, other.geom);
		std::ranges::swap(frag, other.frag);
	}

	auto shader_program::vertex_part_is(const std::filesystem::path& file) const noexcept -> bool {
		return vert and vert->associated_file_ == file;
	}

	auto shader_program::geometry_part_is(const std::filesystem::path& file) const noexcept -> bool {
		return geom and geom->associated_file_ == file;
	}

	auto shader_program::fragment_part_is(const std::filesystem::path& file) const noexcept -> bool {
		return frag and frag->associated_file_ == file;
	}

	auto shader_program::get_id() const noexcept -> uint_t {
		return program_id;
	}

	auto shader_program::bind_uniform_block(std::string_view name, uint_t binding_point) const -> void {
		auto location = get_uniform_block_location(name);
		nifo_gl_invoke(glUniformBlockBinding, program_id, location, binding_point);
	}

	auto shader_program::use() const -> void {
		nifo_gl_invoke(glUseProgram, program_id);
	}

	auto shader_program::check_program_link_error_() ->void {
		int_t errc;
		nifo_gl_invoke(glGetProgramiv, program_id, GL_LINK_STATUS, &errc);
		if (!errc) {
			int_t len;
			nifo_gl_invoke(glGetProgramiv, program_id, GL_INFO_LOG_LENGTH, &len);
			std::vector<char> errors(len);
			nifo_gl_invoke(glGetProgramInfoLog, program_id, len, &len, errors.data());
			errors.emplace_back('\0');
			throw runtime_error{"[Shader Linking Error] " + std::string{errors.begin(), errors.end()}};
		}
	}
	
	auto shader_program::check_program_valid_() -> void {
		int_t errc;
		nifo_gl_invoke(glGetProgramiv, program_id, GL_VALIDATE_STATUS, &errc);
		if (!errc) {
			int_t len;
			nifo_gl_invoke(glGetProgramiv, program_id, GL_INFO_LOG_LENGTH, &len);
			std::vector<char> errors(len);
			nifo_gl_invoke(glGetProgramInfoLog, program_id, len, &len, errors.data());
			errors.emplace_back('\0');
			nifo_gl_invoke(glDeleteProgram, program_id);
			throw runtime_error{"[Shader Program Invalid] " + std::string{errors.begin(), errors.end()}};
		}
	}

	auto shader_program::get_attribute_location(std::string_view name) const -> nifo::int_t {
		if (auto loc = nifo_gl_invoke(glGetAttribLocation, program_id, name.data()); loc == -1) throw runtime_error{fmt::format("the attribute `{}` starts with the reserved prefix \"gl_\" or is not in the shader program", name)};
		else return loc;
	}
	
	auto shader_program::get_uniform_location(std::string_view name) const -> nifo::int_t {
		if (!shader_program_uniform_location_cached.contains(program_id)) {
			shader_program_uniform_location_cached[program_id] = {};
		}

		if (shader_program_uniform_location_cached[program_id].contains(name)) {
			return shader_program_uniform_location_cached[program_id][std::string{name}];
		}

		auto loc = nifo_gl_invoke(glGetUniformLocation, program_id, name.data());
		shader_program_uniform_location_cached[program_id][std::string{name}] = loc;
		return loc;
	}

	auto shader_program::get_uniform_block_location(std::string_view name) const ->nifo::uint_t {
		if (!shader_program_uniform_block_location_cached.contains(program_id)) {
			shader_program_uniform_block_location_cached[program_id] = {};
		}

		if (shader_program_uniform_block_location_cached[program_id].contains(name)) {
			return shader_program_uniform_block_location_cached[program_id][std::string{name}];
		}

		if (auto loc = nifo_gl_invoke(glGetUniformBlockIndex, program_id, name.data()); loc == GL_INVALID_INDEX) throw runtime_error{fmt::format("the parameter `{}` does not identify an active uniform block", name)};
		else {
			shader_program_uniform_block_location_cached[program_id][std::string{name}] = loc;
			return loc;
		}
	}
}
