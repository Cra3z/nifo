#include <array>
#include <nifo/core/ogl/texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace nifo {
	auto internal_format_to_external_format(nifo::texel_format internal_fmt) ->std::tuple<nifo::channel, std::size_t, GLenum> {
		using enum nifo::texel_format;
		using enum nifo::channel;
		static const std::unordered_map<nifo::texel_format, std::tuple<nifo::channel, std::size_t, GLenum>> dict{
			{grey8,     {grey,  1, GL_UNSIGNED_BYTE}},
			{grey16,    {grey,  2, GL_UNSIGNED_SHORT}},
			{grey16f,   {grey,  2, GL_HALF_FLOAT}},
			{grey32f,   {grey,  4, GL_FLOAT}},

			{rgb8,     {rgb,    3, GL_UNSIGNED_BYTE}},
			{rgba8,    {rgba,   4, GL_UNSIGNED_BYTE}},

			{rgb16,    {rgb,    6, GL_UNSIGNED_SHORT}},
			{rgba16,   {rgb,    8, GL_UNSIGNED_SHORT}},

			{rgb16f,    {rgb,   6, GL_HALF_FLOAT}},
			{rgba16f,   {rgb,   8, GL_HALF_FLOAT}},

			{rgb32f,    {rgb,   12, GL_FLOAT}},
			{rgba32f,   {rgb,   16, GL_FLOAT}},

			{srgb8,    {rgb,    3, GL_UNSIGNED_BYTE}},
			{srgba8,   {rgba,   4, GL_UNSIGNED_BYTE}},

			{depth16,  {depth,  2, GL_UNSIGNED_SHORT}},
			{depth24,  {depth,  4, GL_UNSIGNED_INT}},
			{depth32,  {depth,  4, GL_UNSIGNED_INT}},
			{depth32f, {depth,  4, GL_FLOAT}},

			{stencil8,  {stencil, 1, GL_UNSIGNED_BYTE}},
			{stencil16, {stencil, 2, GL_UNSIGNED_SHORT}},
	 	};
		return dict.at(internal_fmt);
	};
	
	auto stb_texture_data_deleter = [](unsigned char* data) ->void {
		::stbi_image_free(data);
	};

	template<texture_type TextureType>
	basic_texture<TextureType>::basic_texture(int) noexcept {
		nifo_gl_invoke(glCreateTextures, to_underlying(TextureType), 1, &id);
	}

	template<texture_type TextureType>
	basic_texture<TextureType>::basic_texture(uint_t id_, texture_wrapping_policies<TextureType> wrapping, texture_filtering_policies filtering, texel_format internal_format, ssize_t width, ssize_t height, ssize_t depth, bool mipmap) noexcept : id(id_),  wrapping_(wrapping), filtering_(filtering), width_(width), height_(height), depth_(depth), format_(internal_format), mipmap_generated_(mipmap) {}

	template<>
	basic_texture<texture_type::two_dimensional>::basic_texture(const basic_texture& other) : basic_texture(0) {
		set_wrapping_policies(other.wrapping_);
		set_filtering_policies(other.filtering_);
		width_ = other.width_;
		height_ = other.height_;
		depth_ = other.depth_;
		format_ = other.format_;
		mipmap_generated_ = other.mipmap_generated_;
		auto [data_format, data_size, data_type] = internal_format_to_external_format(format_);
		std::vector<std::byte> buffer(width_ * height_ * data_size);
		other.copy_to(buffer);
		nifo_gl_invoke(glTextureSubImage2D, id, 0, 0, 0, width_, height_, to_underlying(data_format), data_type, std::ranges::data(buffer));
		if (mipmap_generated_) nifo_gl_invoke(glGenerateTextureMipmap, id);
	};
	
	template<texture_type TextureType>
	basic_texture<TextureType>::basic_texture(basic_texture&& other) noexcept : id(std::exchange(other.id, {})), width_(std::exchange(other.width_, {})), height_(std::exchange(other.height_, {})), depth_(std::exchange(other.depth_, {})), format_(std::exchange(other.format_, {})), mipmap_generated_(std::exchange(other.mipmap_generated_, {})) {}
	
	template<texture_type TextureType>
	basic_texture<TextureType>::~basic_texture() {
		nifo_gl_invoke(glDeleteTextures, 1, &id);
	}

	template<>
	auto basic_texture<texture_type::two_dimensional>::operator= (const basic_texture& other) ->basic_texture& {
		basic_texture{other}.swap(*this);
		return *this;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::operator= (basic_texture&& other) noexcept ->basic_texture& {
		basic_texture{std::move(other)}.swap(*this);
		return *this;
	}
	
	template<texture_type TextureType>
	auto basic_texture<TextureType>::swap(basic_texture& other) noexcept ->void {
		std::ranges::swap(id, other.id);
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::bind_unit(uint_t unit) const -> void {
		nifo_gl_invoke(glBindTextureUnit, unit, id);
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::get_id() const noexcept ->uint_t {
		return id;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::valid() const noexcept ->bool {
		return id != 0;
	}

	template<texture_type TextureType>
	basic_texture<TextureType>::operator bool() const noexcept {
		return id != 0;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::wrapping_policies() const noexcept -> texture_wrapping_policies<TextureType> {
		return wrapping_;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::filtering_policies() const noexcept -> texture_filtering_policies {
		return filtering_;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::width() const noexcept -> ssize_t {
		return width_;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::height() const noexcept -> ssize_t {
		return height_;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::depth() const noexcept -> ssize_t {
		return depth_;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::release() noexcept -> ssize_t {
		return std::exchange(id, {});
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::set_wrapping_policies(texture_wrapping_policies<TextureType> wrapping) ->void {
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_WRAP_S, to_underlying(wrapping.s));
		if constexpr (TextureType != texture_type::one_dimensional) {
			nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_WRAP_T, to_underlying(wrapping.t));
		}
		if constexpr (TextureType == texture_type::cube || TextureType == texture_type::three_dimensional) {
			nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_WRAP_R, to_underlying(wrapping.r));
		}
		wrapping_ = wrapping;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::set_filtering_policies(texture_filtering_policies filtering) ->void {
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_MAG_FILTER, to_underlying(filtering.on_magnify));
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_MIN_FILTER, to_underlying(filtering.on_minify));
		filtering_ = filtering;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::generate_if_without_mipmap() -> void {
		if (!mipmap_generated_) nifo_gl_invoke(glGenerateTextureMipmap, id);
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::format() const noexcept -> texel_format {
		return format_;
	}

	template<texture_type TextureType>
	auto basic_texture<TextureType>::mipmap_generated() const noexcept -> bool {
		return mipmap_generated_;
	}

	auto create_texture2D_impl(
		bool gen_mipmap, unique_image_data& data, int width, int height,
		texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping,
		texel_format internal_format, channel data_format
	) ->texture2d {
		texture2d result{0};

		nifo_gl_invoke(glTextureParameteri, result.id, GL_TEXTURE_MAG_FILTER, to_underlying(filtering.on_magnify));
		nifo_gl_invoke(glTextureParameteri, result.id, GL_TEXTURE_MIN_FILTER, to_underlying(filtering.on_minify));

		nifo_gl_invoke(glTextureParameteri, result.id, GL_TEXTURE_WRAP_S, to_underlying(wrapping.s));
		nifo_gl_invoke(glTextureParameteri, result.id, GL_TEXTURE_WRAP_T, to_underlying(wrapping.t));

		nifo_gl_invoke(glTextureStorage2D, result.id, 1, to_underlying(internal_format), width, height);
		nifo_gl_invoke(glTextureSubImage2D, result.id, 0, 0, 0, width, height, to_underlying(data_format), GL_UNSIGNED_BYTE, data.get());
		if (gen_mipmap) nifo_gl_invoke(glGenerateTextureMipmap, result.id);
		result.wrapping_ = wrapping;
		result.filtering_ = filtering;
		result.width_ = width;
		result.height_ = height;
		result.depth_ = 1;
		result.format_ = internal_format;
		result.mipmap_generated_ = gen_mipmap;
		return result;
	}

	template<>
	auto texture_manager<texture_type::two_dimensional>::load(const std::filesystem::path& texture_path, bool flip_vertically, texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping, texel_format internal_format, bool mipmap) ->const managed_texture_type& {
		if (not dict_.contains(texture_path)) {
			auto [data, width, height, data_format] = load_image(texture_path, flip_vertically);
			if (internal_format == texel_format::automatic) {
				switch (data_format) {
				case channel::grey:
					internal_format = texel_format::grey8;
					break;
				case channel::rgb:
					internal_format = texel_format::rgb8;
					break;
				case channel::rgba:
					internal_format = texel_format::rgba8;
					break;
				default: unreachable();
				}
			}
			dict_.try_emplace(
				texture_path,
				create_texture2D_impl(
					mipmap, data, width, height,
					filtering, wrapping, internal_format, data_format
				)
			);
		}
		return dict_.find(texture_path)->second;
	}

	template<>
	auto texture_manager<texture_type::two_dimensional>::reload(const std::filesystem::path& texture_path, bool flip_vertically, texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping, texel_format internal_format, bool mipmap) ->const managed_texture_type& {
		if (dict_.contains(texture_path)) unload(texture_path);
		auto [data, width, height, data_format] = load_image(texture_path, flip_vertically);
		if (internal_format == texel_format::automatic) {
			switch (data_format) {
			case channel::grey:
				internal_format = texel_format::grey8;
				break;
			case channel::rgb:
				internal_format = texel_format::rgb8;
				break;
			case channel::rgba:
				internal_format = texel_format::rgba8;
				break;
			default: unreachable();
			}
		}
		dict_.try_emplace(
			texture_path,
			create_texture2D_impl(
				mipmap, data, width, height,
				filtering, wrapping, internal_format, data_format
			)
		);
		return dict_.find(texture_path)->second;
	}

	template<texture_type TextureType>
	auto texture_manager<TextureType>::operator[](const std::filesystem::path& texture_path) ->managed_texture_type& {
		try {
			auto& result = dict_.at(texture_path);
			return result;
		}
		catch (const std::out_of_range&) {
			throw runtime_error{fmt::format("the texture \"{}\" not managed", texture_path)};
		}
	}

	template<texture_type TextureType>
	auto texture_manager<TextureType>::contains(const std::filesystem::path& texture_path) noexcept -> bool {
		return dict_.contains(texture_path);
	}

	template<texture_type TextureType>
	auto texture_manager<TextureType>::unload(const std::filesystem::path& texture_path) ->void {
		dict_.erase(texture_path);
	}

	template<texture_type TextureType>
	auto texture_manager<TextureType>::extract(const std::filesystem::path& texture_path) ->std::optional<managed_texture_type> {
		if (dict_.contains(texture_path)) {
			managed_texture_type tmp = std::move(dict_[texture_path]);
			unload(texture_path);
			return std::optional{std::move(tmp)};
		}
		return {};
	}

	template<texture_type TextureType>
	auto texture_manager<TextureType>::clear() -> void {
		dict_.clear();
	}

	template<texture_type TextureType>
	auto texture_manager<TextureType>::reload(const std::filesystem::path& texture_path, managed_texture_type texture) -> const
		managed_texture_type& {
		return dict_[texture_path] = std::move(texture);
	}

	auto create_texture2d_without_mipmap(
		unique_image_data& data, int width, int height,
		texel_format internal_format, channel data_format,
		texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping
	) ->texture2d {
		return create_texture2D_impl(false, data, width, height, filtering, wrapping, internal_format, data_format);
	}

	auto create_texture2d(
		unique_image_data& data, int width, int height,
		texel_format internal_format, channel data_format,
		texture_filtering_policies filtering, texture_wrapping_policies<texture_type::two_dimensional> wrapping
	) ->texture2d {
		return create_texture2D_impl(true, data, width, height, filtering, wrapping, internal_format, data_format);
	}

	auto create_texture2d_storage(
		int width, int height,
		texel_format internal_format,
		texture_filtering_policies filtering
	) ->texture2d {

		texture2d result{0};

		nifo_gl_invoke(glTextureParameteri, result.id, GL_TEXTURE_MAG_FILTER, to_underlying(filtering.on_magnify));
		nifo_gl_invoke(glTextureParameteri, result.id, GL_TEXTURE_MIN_FILTER, to_underlying(filtering.on_minify));

		nifo_gl_invoke(glTextureStorage2D, result.id, 1, to_underlying(internal_format), width, height);
		result.filtering_ = filtering;
		result.width_ = width;
		result.height_ = height;
		result.format_ = internal_format;
		return result;
	}

	auto create_cubemap_from_texture2ds(
		const basic_texture<texture_type::two_dimensional>& pos_x, const basic_texture<texture_type::two_dimensional>& neg_x,
		const basic_texture<texture_type::two_dimensional>& pos_y, const basic_texture<texture_type::two_dimensional>& neg_y,
		const basic_texture<texture_type::two_dimensional>& pos_z, const basic_texture<texture_type::two_dimensional>& neg_z,
		texel_format internal_format, bool gen_mipmap,
		texture_filtering_policies filtering, texture_wrapping_policies<texture_type::cube> wrapping
	) ->basic_texture<texture_type::cube> {
		GLuint id;
		nifo_gl_invoke(glCreateTextures, GL_TEXTURE_CUBE_MAP, 1, &id);
		auto width = pos_x.width();
		auto height = pos_x.height();
		nifo_gl_invoke(glTextureStorage2D, id, 1, to_underlying(internal_format), width, height);
		std::array packed_cube_face{
			&pos_x, &neg_x,
			&pos_y, &neg_y,
			&pos_z, &neg_z
		};
		auto [data_format, data_size, data_type] = internal_format_to_external_format(pos_x.format());
		std::vector<std::byte> buffer(width * height * data_size);
		for (int i = 0; i < 6; ++i) {
			packed_cube_face[i]->copy_to(buffer);
			nifo_gl_invoke(glTextureSubImage3D, id, 0, 0, 0, i, width, height, 1, to_underlying(data_format), data_type, std::ranges::data(buffer));
		}
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_MAG_FILTER, to_underlying(filtering.on_magnify));
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_MIN_FILTER, to_underlying(filtering.on_minify));
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_WRAP_S, to_underlying(wrapping.s));
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_WRAP_T, to_underlying(wrapping.t));
		nifo_gl_invoke(glTextureParameteri, id, GL_TEXTURE_WRAP_R, to_underlying(wrapping.r));
		if (gen_mipmap) nifo_gl_invoke(glGenerateTextureMipmap, id);
		return {id, wrapping, filtering, internal_format, width, height, 6, gen_mipmap};
	}

	auto create_cubemap_from_paths(
		texture_manager<texture_type::two_dimensional>& texture2d_manager,
		const std::filesystem::path& pos_x, const std::filesystem::path& neg_x,
		const std::filesystem::path& pos_y, const std::filesystem::path& neg_y,
		const std::filesystem::path& pos_z, const std::filesystem::path& neg_z,
		texel_format internal_format, bool gen_mipmap,
		texture_filtering_policies filtering, texture_wrapping_policies<texture_type::cube> wrapping
	) ->basic_texture<texture_type::cube> {
		return create_cubemap_from_texture2ds(
			texture2d_manager.load(pos_x, false),
			texture2d_manager.load(neg_x, false),
			texture2d_manager.load(pos_y, false),
			texture2d_manager.load(neg_y, false),
			texture2d_manager.load(pos_z, false),
			texture2d_manager.load(neg_z, false),
			internal_format, gen_mipmap,
			filtering, wrapping
		);
	}

	auto load_image(const std::filesystem::path& texture_path, bool flip_vertically) ->std::tuple<unique_image_data, int, int, channel> {
		int width, height, channels;
		::stbi_set_flip_vertically_on_load(flip_vertically);
		auto data_ = ::stbi_load(texture_path.string().c_str(), &width, &height, &channels, 0);
		if (data_ == nullptr) throw runtime_error{fmt::format(R"(failed to load texture: "{}")", texture_path)};
		channel data_format;
		switch (channels) {
		case STBI_grey:
			data_format = channel::grey;
			break;
		case STBI_rgb:
			data_format = channel::rgb;
			break;
		case STBI_rgb_alpha:
			data_format = channel::rgba;
			break;
		default:
			throw runtime_error{fmt::format(R"(the channel type of "{}" is unsupported)", texture_path)};
		}
		return {unique_image_data{data_, stb_texture_data_deleter}, width, height, data_format};
	}

	template class basic_texture<texture_type::two_dimensional>;
	template class basic_texture<texture_type::cube>;
	template class texture_manager<texture_type::two_dimensional>;
}