#pragma once
#include <vector>
#include <filesystem>
#include <map>
#include <optional>
#include "vertex_buffer.h"

namespace nifo {
	
	enum class texture_type : GLenum {
		one_dimensional = GL_TEXTURE_1D,   // 1D texture
		two_dimensional = GL_TEXTURE_2D,   // 1D texture
		three_dimensional = GL_TEXTURE_3D, // 3D texture
		cube = GL_TEXTURE_CUBE_MAP         // cube map texture
	};
	
	enum class filtering_policy : GLint {
		nearest = GL_NEAREST,
		linear = GL_LINEAR,
		nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
		nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
		linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
		linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR
	};
	
	enum class wrapping_policy : GLint {
		repeat = GL_REPEAT,
		clamp = GL_CLAMP,
		clamp_to_edge = GL_CLAMP_TO_EDGE,
		clamp_to_border = GL_CLAMP_TO_BORDER
	};
	
	using unique_image_data = std::unique_ptr<unsigned char, std::function<void(unsigned char*)>>;
	
	template<texture_type>
	struct texture_wrapping_policies;
		
	template<>
	struct texture_wrapping_policies<texture_type::one_dimensional> {
		wrapping_policy s = wrapping_policy::repeat;
	};
		
	template<>
	struct texture_wrapping_policies<texture_type::two_dimensional> {
		wrapping_policy s = wrapping_policy::repeat, t = wrapping_policy::repeat;
	};
		
	template<>
	struct texture_wrapping_policies<texture_type::three_dimensional> {
		wrapping_policy s = wrapping_policy::repeat, t = wrapping_policy::repeat, r = wrapping_policy::repeat;
	};

	template<>
	struct texture_wrapping_policies<texture_type::cube> {
		wrapping_policy s = wrapping_policy::repeat, t = wrapping_policy::repeat, r = wrapping_policy::repeat;
	};
		
	struct texture_filtering_policies {
		filtering_policy on_minify = filtering_policy::nearest, on_magnify = filtering_policy::linear;
	};

	template<texture_type>
	class basic_texture_view;

	auto internal_format_to_external_format(nifo::texel_format internal_fmt) ->std::tuple<nifo::channel, std::size_t, GLenum>;

	// 已限制只能使用已经显式实例化的basic_texture(2D 和 cube 纹理), 1D 和 3D 纹理暂时保留但不实现
	template<texture_type TextureType>
	class basic_texture {

		friend auto create_texture2D_impl(
			bool gen_mipmap, unique_image_data& data, int width, int height,
			texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping,
			texel_format internal_format, channel data_format
		) ->basic_texture<texture_type::two_dimensional>;

		friend auto create_texture2d_storage(
			int width, int height, texel_format internal_format,
			texture_filtering_policies filtering
		) ->basic_texture<texture_type::two_dimensional>;

	private:

		basic_texture(int) noexcept;

	public:

		template<texture_type>
		friend class basic_texture_view;

		basic_texture() = default;

		basic_texture(uint_t id_, texture_wrapping_policies<TextureType> wrapping, texture_filtering_policies filtering, texel_format internal_format, ssize_t width, ssize_t height, ssize_t depth, bool mipmap) noexcept;

		basic_texture(const basic_texture& other) = delete;

		basic_texture(basic_texture&& other) noexcept;

		~basic_texture();

		auto operator= (const basic_texture& other) ->basic_texture& = delete;

		auto operator= (basic_texture&& other) noexcept ->basic_texture&;

		auto swap(basic_texture& other) noexcept ->void;

		auto friend swap(basic_texture& lhs, basic_texture& rhs) noexcept ->void {
			lhs.swap(rhs);
		}

		auto bind_unit(uint_t unit) const ->void;

		[[nodiscard]]
		auto get_id() const noexcept ->uint_t;

		[[nodiscard]]
		auto valid() const noexcept ->bool;

		explicit operator bool() const noexcept;

		[[nodiscard]]
		auto wrapping_policies() const noexcept ->texture_wrapping_policies<TextureType> ;

		[[nodiscard]]
		auto filtering_policies() const noexcept ->texture_filtering_policies ;

		[[nodiscard]]
		auto width() const noexcept ->ssize_t;

		[[nodiscard]]
		auto height() const noexcept ->ssize_t;

		[[nodiscard]]
		auto depth() const noexcept ->ssize_t;

		[[nodiscard]]
		auto format() const noexcept ->texel_format;

		[[nodiscard]]
		auto mipmap_generated() const noexcept ->bool;

		[[nodiscard]]
		auto release() noexcept ->ssize_t;

		auto set_wrapping_policies(texture_wrapping_policies<TextureType> wrapping) ->void;

		auto set_filtering_policies(texture_filtering_policies filtering) ->void;

		auto generate_if_without_mipmap() ->void;

		template<typename Buffer> requires std::ranges::contiguous_range<Buffer> && std::ranges::sized_range<Buffer> && std::ranges::borrowed_range<Buffer> && std::is_trivially_copyable_v<std::ranges::range_value_t<Buffer>>
		auto copy_to(Buffer&& buffer, GLenum level = 0) const ->void {
			auto [channel_, ignored, data_type] = internal_format_to_external_format(format_);
			nifo_gl_invoke(glGetTextureImage, id, level, to_underlying(channel_), data_type, std::ranges::size(buffer) * sizeof(std::ranges::range_value_t<Buffer>), std::ranges::data(buffer));
		}
	private:
		uint_t id{};
		texture_wrapping_policies<TextureType> wrapping_;
		texture_filtering_policies filtering_;
		ssize_t width_{}, height_{}, depth_{};
		texel_format format_{};
		bool mipmap_generated_{};
	};

	template<>
	basic_texture<texture_type::two_dimensional>::basic_texture(const basic_texture& other);

	template<>
	auto basic_texture<texture_type::two_dimensional>::operator= (const basic_texture& other) ->basic_texture&;

	struct unsupported_texel_format : runtime_error {
		using runtime_error::runtime_error;
	};

	// template<texture_type TextureType>
	// class basic_texture_view {
	// public:
	// 	basic_texture_view(texture_type<TextureType>& texture, color_format internal_format, uint_t minlevel, uint_t numlevels, uint_t minlayer, uint_t numlayers) {
	// 		::glCreateTextures(to_underlying(TextureType), 1, &id);
	// 		check_error(glTextureView, id, to_underlying(TextureType), texture.id_, to_underlying(internal_format), minlevel, numlevels, minlayer, numlayers);
	// 	}
	// 	~basic_texture_view() {
	// 		::glDeleteTextures(1, &id);
	// 	}
	// private:
	// 	uint_t id{};
	// };

	template<texture_type TextureType>
	class texture_manager {
	public:

		using managed_texture_type = basic_texture<TextureType>;

	public:

		texture_manager() = default;

		texture_manager(const texture_manager&) = delete;

		~texture_manager() = default;

		auto operator= (const texture_manager&) ->texture_manager& = delete;

		/**
		 * @brief 载入纹理
		 *
		 * 若未托管@p texture_path 纹理则使用参数创建一个纹理对象, 若已托管则直接返回纹理对象的引用
		 *
		 * @param texture_path 纹理路径
		 * @param flip_vertically 是否垂直翻转
		 * @param filtering 纹理过滤方式
		 * @param wrapping 纹理环绕方式
		 * @param internal_format 纹理内部格式
		 * @param mipmap 是否生成mipmap
		 *
		 * @return 纹理对象的引用
		 */
		auto load(const std::filesystem::path& texture_path, bool flip_vertically = true, texture_filtering_policies filtering = {.on_minify = filtering_policy::nearest_mipmap_linear}, texture_wrapping_policies<TextureType> wrapping = {}, texel_format internal_format = texel_format::automatic, bool mipmap = true) ->const managed_texture_type&;

		/**
		 * @brief 重载入纹理
		 *
		 * 若未托管@p texture_path 纹理则使用参数创建一个纹理对象, 若已托管则销毁掉之前的纹理对象再重新使用参数创建一个
		 *
		 * @param texture_path 纹理路径
		 * @param flip_vertically 是否垂直翻转
		 * @param filtering 纹理过滤方式
		 * @param wrapping 纹理环绕方式
		 * @param internal_format 纹理内部格式
		 * @param mipmap 是否生成mipmap
		 *
		 * @return 纹理对象的引用
		 */
		auto reload(const std::filesystem::path& texture_path, bool flip_vertically = true, texture_filtering_policies filtering = {.on_minify = filtering_policy::nearest_mipmap_linear}, texture_wrapping_policies<TextureType> wrapping = {}, texel_format internal_format = texel_format::automatic, bool mipmap = true) ->const managed_texture_type&;

		/**
		 * @brief 重载入纹理
		 *
		 * 将纹理对象@p texture 的所有权移动给@p texture_path 关联的纹理对象, 若@p texture_path 无关联的纹理对象则先创建一个与之关联的空纹理对象再转移@p texture的所有权给该空纹理对象
		 * @param texture_path 纹理路径
		 * @param texture 纹理对象
		 *
		 * @return 纹理对象的引用
		 */
		auto reload(const std::filesystem::path& texture_path, managed_texture_type texture) -> const managed_texture_type&;


		auto operator[] (const std::filesystem::path& texture_path) ->managed_texture_type&;

		/**
		 * @brief 是否已管理该路径的纹理
		 * @param texture_path 纹理路径
		 * @return 若该纹理路径已关联某个纹理对象则返回true, 否则返回false
		 */
		auto contains(const std::filesystem::path& texture_path) noexcept ->bool;

		auto unload(const std::filesystem::path& texture_path) ->void;

		/**
		 * @brief 取消对某个纹理文件所关联的纹理对象的管理
		 * @param texture_path 纹理路径
		 * @return 若texture manager已经管理该纹理对象则返回持有该对象的optional否则返回nullopt
		 */
		[[nodiscard]] auto extract(const std::filesystem::path& texture_path) ->std::optional<managed_texture_type>;

		auto clear() ->void;
	private:
		std::map<std::filesystem::path, managed_texture_type, std::less<>> dict_;
	};

	template<>
	auto texture_manager<texture_type::two_dimensional>::load(const std::filesystem::path& texture_path, bool flip_vertically, texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping, texel_format internal_format, bool mipmap) ->const managed_texture_type&;

	template<>
	auto texture_manager<texture_type::two_dimensional>::reload(const std::filesystem::path& texture_path, bool flip_vertically, texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping, texel_format internal_format, bool mipmap) ->const managed_texture_type&;

	extern template class basic_texture<texture_type::two_dimensional>;

	extern template class basic_texture<texture_type::cube>;

	extern template class texture_manager<texture_type::two_dimensional>;

	using texture2d = basic_texture<texture_type::two_dimensional>;

	using cube_map = basic_texture<texture_type::cube>;

	using texture2d_manager = texture_manager<texture_type::two_dimensional>;

	[[nodiscard]]
	auto load_image(const std::filesystem::path& texture_path, bool flip_vertically = true) ->std::tuple<unique_image_data, int, int, channel>;

	[[nodiscard]]
	auto create_texture2d_without_mipmap(
		unique_image_data& data, int width, int height,
		texel_format internal_format = texel_format::rgb8, channel data_format = channel::rgb,
		texture_filtering_policies filtering = {}, texture_wrapping_policies<texture_type::two_dimensional> wrapping = {}
	) ->texture2d;

	[[nodiscard]]
	auto create_texture2d(
		unique_image_data& data, int width, int height,
		texel_format internal_format = texel_format::rgb8, channel data_format = channel::rgb,
		texture_filtering_policies filtering = {.on_minify = filtering_policy::nearest_mipmap_linear}, texture_wrapping_policies<texture_type::two_dimensional> wrapping = {}
	) ->texture2d;

	[[nodiscard]]
	auto create_texture2d_storage(
		int width, int height, texel_format internal_format = texel_format::rgb8,
		texture_filtering_policies filtering = {.on_minify = filtering_policy::linear, .on_magnify = filtering_policy::linear}
	) ->texture2d;

	[[nodiscard]]
	auto create_cubemap_from_texture2ds( // assume: 所有的纹理都具有相同的宽高
		basic_texture<texture_type::two_dimensional>& pos_x, basic_texture<texture_type::two_dimensional>& neg_x,
		basic_texture<texture_type::two_dimensional>& pos_y, basic_texture<texture_type::two_dimensional>& neg_y,
		basic_texture<texture_type::two_dimensional>& pos_z, basic_texture<texture_type::two_dimensional>& neg_z,
		texel_format internal_format = texel_format::rgb8, bool mipmap = true,
		texture_filtering_policies filtering = {.on_minify = filtering_policy::nearest_mipmap_linear}, texture_wrapping_policies<texture_type::cube> wrapping = {.s = wrapping_policy::clamp_to_edge, .t = wrapping_policy::clamp_to_edge, .r = wrapping_policy::clamp_to_edge}
	) ->basic_texture<texture_type::cube>;

	[[nodiscard]]
	auto create_cubemap_from_paths( // assume: 所有的纹理都具有相同的宽高
		texture_manager<texture_type::two_dimensional>& texture2d_manager,
		const std::filesystem::path& pos_x, const std::filesystem::path& neg_x,
		const std::filesystem::path& pos_y, const std::filesystem::path& neg_y,
		const std::filesystem::path& pos_z, const std::filesystem::path& neg_z,
		texel_format internal_format = texel_format::rgb8, bool gen_mipmap = true,
		texture_filtering_policies filtering = {.on_minify = filtering_policy::nearest_mipmap_linear}, texture_wrapping_policies<texture_type::cube> wrapping = {.s = wrapping_policy::clamp_to_edge, .t = wrapping_policy::clamp_to_edge, .r = wrapping_policy::clamp_to_edge}
	) ->basic_texture<texture_type::cube>;
}
