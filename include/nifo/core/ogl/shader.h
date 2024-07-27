#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertex_buffer.h"
#include "nifo/core/ecs/assets_loader.h"

namespace nifo {
	enum class shader_type : GLenum {
		none = 0,
		vertex_shader =  GL_VERTEX_SHADER,
		fragment_shader = GL_FRAGMENT_SHADER,
		geometry_shader = GL_GEOMETRY_SHADER,
		compute_shader = GL_COMPUTE_SHADER,
		tess_control_shader = GL_TESS_CONTROL_SHADER,
		tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
	};
	
	class shader_part {
		
		friend class shader_program;
		
	public:
		
		shader_part() = default;
		
		shader_part(std::filesystem::path file_path);
		
		shader_part(const shader_part&) = delete;
		
		shader_part(shader_part&& other) noexcept;
		
		auto operator= (const shader_part&) ->shader_part& = delete;
		
		auto operator= (shader_part&& other) noexcept ->shader_part&;
		
		~shader_part();

		auto swap(shader_part& other) noexcept ->void;
		
		friend auto swap(shader_part& lhs, shader_part& rhs) noexcept ->void {
			lhs.swap(rhs);
		}

		[[nodiscard]]
		auto type() const noexcept ->shader_type;

		[[nodiscard]]
		auto associated_file() const noexcept ->const std::filesystem::path&;

	private:

		static auto recongnize_file_type_(const std::filesystem::path& file_path) ->shader_type;

		auto check_shader_error_() ->void;
		
	private:
		std::filesystem::path associated_file_;
		uint_t shader_id{};
		shader_type type_{shader_type::none};
	};
	
	class shader_program {

		shader_program(int) noexcept;

	public:
		
		shader_program() = default;
		
		shader_program(std::shared_ptr<shader_part> vert_shader, std::shared_ptr<shader_part> frag_shader);

		shader_program(std::shared_ptr<shader_part> vert_shader, std::shared_ptr<shader_part> geom_shader, std::shared_ptr<shader_part> frag_shader);

		shader_program(const shader_program& other);
		
		shader_program(shader_program&& other) noexcept;
		
		~shader_program();
		
		auto operator= (const shader_program& other) ->shader_program&;
		
		auto operator= (shader_program&& other) noexcept ->shader_program&;
		
		auto swap(shader_program& other) noexcept ->void;
		
		friend auto swap(shader_program& lhs, shader_program& rhs) noexcept ->void {
			lhs.swap(rhs);
		}

		[[nodiscard]]
		auto vertex_part_is(const std::filesystem::path& file) const noexcept ->bool;

		[[nodiscard]]
		auto geometry_part_is(const std::filesystem::path& file) const noexcept ->bool;

		[[nodiscard]]
		auto fragment_part_is(const std::filesystem::path& file) const noexcept ->bool;

		[[nodiscard]]
		auto get_id() const noexcept ->uint_t;

		[[nodiscard]]
		auto get_attribute_location(std::string_view name) const ->nifo::int_t;

		[[nodiscard]]
		auto get_uniform_location(std::string_view name) const ->nifo::int_t;

		[[nodiscard]]
		auto get_uniform_block_location(std::string_view name) const ->nifo::uint_t;
		
		template<typename V> requires meta::type_list<float_t, double_t, int_t, uint_t, bool>::containes<V>
		auto try_set_uniform(std::string_view name, const V& v) const ->bool {
			auto loc = get_uniform_location(name);
			if (loc == -1) return false;
			if constexpr (std::same_as<V, float_t>) {
				nifo_gl_invoke(glProgramUniform1f, program_id, loc, v);
			}
			else if constexpr (std::same_as<V, double_t>) {
				nifo_gl_invoke(glProgramUniform1d, program_id, loc, v);
			}
			else if constexpr (std::same_as<V, int_t>) {
				nifo_gl_invoke(glProgramUniform1i, program_id, loc, v);
			}
			else if constexpr (std::same_as<V, uint_t>) {
				nifo_gl_invoke(glProgramUniform1ui, program_id, loc, v);
			}
			else if constexpr (std::same_as<V, bool>) {
				nifo_gl_invoke(glProgramUniform1i, program_id, loc, v);
			}
			return true;
		}
		
		template<glm::length_t L, typename T, glm::qualifier Q> requires meta::type_list<float_t, double_t, int_t, uint_t>::containes<T>
		auto try_set_uniform(std::string_view name, const glm::vec<L, T, Q>& v) const ->bool {
			auto loc = get_uniform_location(name);
			if (loc == -1) return false;
			if constexpr (std::same_as<T, float_t>) {
				if constexpr      (L == 1) nifo_gl_invoke(glProgramUniform1fv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 2) nifo_gl_invoke(glProgramUniform2fv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 3) nifo_gl_invoke(glProgramUniform3fv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 4) nifo_gl_invoke(glProgramUniform4fv, program_id, loc, 1, glm::value_ptr(v));
			}
			else if constexpr (std::same_as<T, double_t>) {
				if constexpr      (L == 1) nifo_gl_invoke(glProgramUniform1dv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 2) nifo_gl_invoke(glProgramUniform2dv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 3) nifo_gl_invoke(glProgramUniform3dv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 4) nifo_gl_invoke(glProgramUniform4dv, program_id, loc, 1, glm::value_ptr(v));
			}
			else if constexpr (std::same_as<T, int_t>) {
				if constexpr      (L == 1) nifo_gl_invoke(glProgramUniform1iv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 2) nifo_gl_invoke(glProgramUniform2iv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 3) nifo_gl_invoke(glProgramUniform3iv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 4) nifo_gl_invoke(glProgramUniform4iv, program_id, loc, 1, glm::value_ptr(v));
			}
			else if constexpr (std::same_as<T, uint_t>) {
				if constexpr      (L == 1) nifo_gl_invoke(glProgramUniform1uiv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 2) nifo_gl_invoke(glProgramUniform2uiv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 3) nifo_gl_invoke(glProgramUniform3uiv, program_id, loc, 1, glm::value_ptr(v));
				else if constexpr (L == 4) nifo_gl_invoke(glProgramUniform4uiv, program_id, loc, 1, glm::value_ptr(v));
			}
			return true;
		}
		
		template<glm::length_t C, glm::length_t R, glm::qualifier Q> requires (C >= 2 and C <= 4)
		auto try_set_uniform(std::string_view name, const glm::mat<C, R, float_t, Q>& m) const ->bool {
			auto loc = get_uniform_location(name);
			if (loc == -1) return false;
			if constexpr     (C == 2) {
				if constexpr      (R == 2) nifo_gl_invoke(glProgramUniformMatrix2fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
				else if constexpr (R == 3) nifo_gl_invoke(glProgramUniformMatrix2x3fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
				else if constexpr (R == 4) nifo_gl_invoke(glProgramUniformMatrix2x4fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
			}
			else if constexpr (C == 3) {
				if constexpr      (R == 2) nifo_gl_invoke(glProgramUniformMatrix3x2fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
				else if constexpr (R == 3) nifo_gl_invoke(glProgramUniformMatrix3fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
				else if constexpr (R == 4) nifo_gl_invoke(glProgramUniformMatrix3x4fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
			}
			else if constexpr (C == 4) {
				if constexpr      (R == 2) nifo_gl_invoke(glProgramUniformMatrix4x2fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
				else if constexpr (R == 3) nifo_gl_invoke(glProgramUniformMatrix4x3fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
				else if constexpr (R == 4) nifo_gl_invoke(glProgramUniformMatrix4fv, program_id, loc, 1, GL_FALSE, glm::value_ptr(m));
			}
			return true;
		}

		template<typename T>
		auto set_uniform(std::string_view name, const T& value) const ->void requires requires {std::declval<shader_program>().try_set_uniform(name, value);} {
			if (not this->try_set_uniform(name, value)) throw runtime_error{fmt::format("the uniform `{}` starts with the reserved prefix \"gl_\" or is not in the shader program", name)};
		}

		auto bind_uniform_block(std::string_view name, uint_t binding_point) const ->void ;

		friend auto operator== (const shader_program& lhs, const shader_program& rhs) noexcept ->bool {
			return lhs.vert == rhs.vert && lhs.geom == rhs.geom && lhs.frag == rhs.frag;
		}

		auto use() const ->void;

	private:
		
		auto check_program_link_error_() ->void;
	
		auto check_program_valid_() ->void;
	private:
		uint_t program_id{};
		std::shared_ptr<shader_part> vert{}, geom{}, frag{};
	};
}
