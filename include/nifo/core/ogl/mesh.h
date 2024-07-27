#pragma once
#include <ranges>
#include <assimp/material.h>
#include "vertex_buffer.h"
#include "texture.h"
#include "shader.h"
#include "../utils/core.h"
#include "../utils/meta.h"
#include "../utils/name_generator.h"

namespace nifo {

	enum class texture_mapping : std::underlying_type_t<aiTextureType> {
		diffuse           = aiTextureType_DIFFUSE,
		normals           = aiTextureType_NORMALS,
		ambient_occlusion = aiTextureType_AMBIENT_OCCLUSION,
		metalness         = aiTextureType_METALNESS,
		roughness         = aiTextureType_DIFFUSE_ROUGHNESS,
		height            = aiTextureType_HEIGHT,
		specular          = aiTextureType_SPECULAR,
		shininess         = aiTextureType_SHININESS,
		ambient           = aiTextureType_AMBIENT,
		opacity           = aiTextureType_OPACITY
	};

	class mesh;

	enum class shading_mode {
		none,
		lambert,
		phong,
		blinn_phong,
		pbr,
		customed
	};

	struct material_base {
		using value_type = std::variant<bool, int, unsigned, float, glm::vec2, glm::vec3, glm::vec4, std::filesystem::path>;

		shading_mode mode = shading_mode::none;

		shader_program shader;

		std::unordered_map<std::string, value_type> attributes;

		friend auto operator== (const material_base& lhs, const material_base& rhs) noexcept ->bool = default;
	};

	class material : public material_base {
	public:

		auto pass_attributes_to_shader(texture2d_manager& texture2d_manager) const ->void;

		static name_generator name_generator;

		mutable int texture_unit = 0;
	};

	class mesh {
	public:

		struct single_buffer_policy_layout {
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> uvs;
		};

		struct vertex_layout {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
		};

		enum class buffer_policy {
			interleaved,
			single
		};
	public:

		mesh(std::vector<vertex_layout> vertices, std::vector<uint_t> indices, std::size_t face_count,  const nifo::material& material);

		/**
		 * @brief 采用single buffer策略保留mesh的顶点数据
		 * @param buffers 对各single buffer的数据, `buffers`中不为空的数组的大小必须一样
		 * @param indices 索引数组
		 * @param face_count 面数
		 * @param material 所用材质
		 */
		mesh(single_buffer_policy_layout buffers, std::vector<uint_t> indices, std::size_t face_count, const nifo::material& material);

		[[nodiscard]]
		auto indices() const noexcept ->const std::vector<uint_t>&;

		[[nodiscard]]
		auto vertex_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto index_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto face_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto material() const noexcept ->const nifo::material&;


		/**
		 * \brief 用mesh自己的材质绘制mesh
		 * \param callback 回调函数, 参数是mesh的材质, 返回值是绘制的图元类型
		 */
		auto draw(const std::function<primitive(const nifo::material&)>& callback) const ->void;

		/**
		 * \brief 用指定的材质(由参数material给出)绘制mesh
		 * \param material 材质
		 * \param callback 回调函数, 参数是material, 返回值是绘制的图元类型
		 */
		auto draw_with(const nifo::material& material, const std::function<primitive(const nifo::material&)>& callback) const ->void;

		[[nodiscard]]
		auto vao() const noexcept ->const vertex_array&;

		[[nodiscard]]
		auto vertex_buffer_policy() const noexcept ->buffer_policy;
	private:
		std::vector<vertex_layout> interleaved_buffer_;
		single_buffer_policy_layout single_buffers_;
		std::vector<uint_t> indices_;
		std::size_t face_count_;
		std::reference_wrapper<const nifo::material> material_;
		vertex_array vao_;
		std::vector<vertex_buffer> vbos_;
		element_buffer ebo_;
		buffer_policy policy_;
	};

	template<typename T>
	concept vertex_which_has_position = requires (T t) {
		t.position;
		{ t.position.x } -> meta::arithmetic;
		{ t.position.y } -> meta::arithmetic;
		{ t.position.z } -> meta::arithmetic;
	};

	template<typename T>
	concept vertex_which_has_normal = requires (T t) {
		t.normal;
		{ t.normal.x } -> meta::arithmetic;
		{ t.normal.y } -> meta::arithmetic;
		{ t.normal.z } -> meta::arithmetic;
	};

	template<typename T>
	concept vertex_which_has_uv = requires (T t) {
		t.uv;
		{ t.uv.x } -> meta::arithmetic;
		{ t.uv.y } -> meta::arithmetic;
	};

	template<typename T>
	concept vertex_which_has_color = requires (T t) {
		t.color;
		{ t.color.x } -> meta::arithmetic;
		{ t.color.y } -> meta::arithmetic;
	};

	template<typename T>
	concept vertex = vertex_which_has_position<T> || vertex_which_has_normal<T> || vertex_which_has_uv<T> || vertex_which_has_color<T>;
}

// NOLINTBEGIN

template<>
struct fmt::formatter<nifo::texture_mapping> {
	constexpr auto parse(fmt::format_parse_context& ctx) {
		auto first = ctx.begin();
		auto last = ctx.end();
		if (first == last || *first == '}') return first;
		throw fmt::format_error{"invalid format option"};
	}

	auto format(const nifo::texture_mapping& type, fmt::format_context& ctx) const {
		using enum nifo::texture_mapping;
		switch (type) {
		case diffuse:
			return fmt::format_to(ctx.out(), "diffuse");
		case normals:
			return fmt::format_to(ctx.out(), "normals");
		case ambient_occlusion:
			return fmt::format_to(ctx.out(), "ambient_occlusion");
		case metalness:
			return fmt::format_to(ctx.out(), "metalness");
		case roughness:
			return fmt::format_to(ctx.out(), "normals");
		case height:
			return fmt::format_to(ctx.out(), "height");
		case specular:
			return fmt::format_to(ctx.out(), "specular");
		case shininess:
			return fmt::format_to(ctx.out(), "shininess");
		case ambient:
			return fmt::format_to(ctx.out(), "ambient");
		case opacity:
			return fmt::format_to(ctx.out(), "opacity");
		default:
			nifo::unreachable();
		}
	}
};

template<>
struct fmt::formatter<nifo::shading_mode> {
	constexpr auto parse(fmt::format_parse_context& ctx) {
		auto first = ctx.begin();
		auto last = ctx.end();
		if (first == last || *first == '}') return first;
		throw fmt::format_error{"invalid format option"};
	}

	auto format(const nifo::shading_mode& mode, fmt::format_context& ctx) const {
		using enum nifo::shading_mode;
		switch (mode) {
		case lambert:
			return fmt::format_to(ctx.out(), "Lambert");
		case phong:
			return fmt::format_to(ctx.out(), "Phong");
		case blinn_phong:
			return fmt::format_to(ctx.out(), "Blinn-Phong");
		case pbr:
			return fmt::format_to(ctx.out(), "PBR");
		default:
			nifo::unreachable();
		}
	}
};

// NOLINTEND