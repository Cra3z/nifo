#pragma once
#include <ranges>
#include <variant>
#include <vector>
#include <set>
#include "vertex_buffer.h"
#include "shader.h"
#include "texture.h"

namespace nifo {

	enum class capability : GLenum {
		depth_test = GL_DEPTH_TEST,
		stencil_test = GL_STENCIL_TEST,
		scissor_test = GL_SCISSOR_TEST,
		alpha_test = GL_ALPHA_TEST,
		color_blending = GL_BLEND,
		face_culling = GL_CULL_FACE,
		multi_sample_anti_aliasing = GL_MULTISAMPLE,
		gamma_correction = GL_FRAMEBUFFER_SRGB,
		seamless_cubemap = GL_TEXTURE_CUBE_MAP_SEAMLESS
	};

	enum class frame_buffer_target : GLenum {
		read_frame_buffer = GL_READ_FRAMEBUFFER,
		draw_frame_buffer = GL_DRAW_FRAMEBUFFER
	};

	enum class default_frame_buffer_source {
		front = GL_FRONT,
		back = GL_BACK,
		front_left = GL_FRONT_LEFT,
		back_left = GL_BACK_LEFT,
		front_right = GL_FRONT_RIGHT,
		back_right = GL_BACK_RIGHT
	};

	class frame_buffer;

	namespace detail {
		struct default_frame_buffer_ {
			friend class ::nifo::frame_buffer;
		public:

			default_frame_buffer_(uint_t);

			default_frame_buffer_(const default_frame_buffer_&) = delete;

			auto operator= (const default_frame_buffer_&) ->default_frame_buffer_& = delete;

			auto transfer_to(frame_buffer& other) ->void;

			auto enable(capability cap) ->default_frame_buffer_&;

			auto disable(capability cap) -> default_frame_buffer_&;

			auto clear_color(glm::vec4 color = {0.f, 0.f, 0.f, 0.f}, int_t color_attachment_index = 0) ->default_frame_buffer_&;

			auto clear_depth(float_t depth = 1.f) ->default_frame_buffer_&;

			auto clear_stencil(int_t stencil = 0) ->default_frame_buffer_&;

			auto set_reading_attachment(default_frame_buffer_source attachment) ->void;

			auto set_drawing_attachment(default_frame_buffer_source attachment) ->void;

			auto bind() const ->void;
		private:
			uint_t id = 0;
			inline static std::map<uint_t, std::set<capability>> info;
		};
	}

	struct use_depth_stencil_common_attachment_t {
		explicit use_depth_stencil_common_attachment_t() = default;
	};

	inline constexpr use_depth_stencil_common_attachment_t use_depth_stencil_common_attachment{};

	class render_buffer {
	public:
		enum class internalformat : GLenum {
			depth24_stencil8 = GL_DEPTH24_STENCIL8
		};

		using enum internalformat;
	public:
		render_buffer() = default;

		explicit render_buffer(uint_t id) noexcept;

		render_buffer(render_buffer&& other) noexcept;

		~render_buffer() noexcept;

		auto operator= (render_buffer&& other) noexcept ->render_buffer& ;

		auto swap(render_buffer& other) noexcept ->void ;

		friend auto swap(render_buffer& lhs, render_buffer& rhs) noexcept ->void {
			lhs.swap(rhs);
		}

		auto bind() const ->void;

		auto unbind() const ->void;

		auto set_storage(internalformat ifmt, ssize_t width, ssize_t height, ssize_t multi_sample = 0) ->void ;

		[[nodiscard]]
		auto get_id() const noexcept ->uint_t ;

	private:
		uint_t id{};
	};

	auto create_render_buffer(render_buffer::internalformat internalformat, ssize_t width, ssize_t height, ssize_t multi_sample = 0) ->render_buffer ;

	class frame_buffer {
		friend class detail::default_frame_buffer_;

		enum class no_color_attachment_t : GLenum {};

	public:

		using color_attachment_t = GLenum;

		constexpr static no_color_attachment_t no_color_attachment{GL_NONE};

		template<int I>
		constexpr static color_attachment_t color_attachment{GL_COLOR_ATTACHMENT0 + I};

		enum class status_type : GLenum {
			complete = GL_FRAMEBUFFER_COMPLETE,
			undefined = GL_FRAMEBUFFER_UNDEFINED,
			unspecified = GL_NONE,
			unsupported = GL_FRAMEBUFFER_UNSUPPORTED,
			incomplete_attachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
			incomplete_missing_attachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
			incomplete_draw_buffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
			incomplete_read_buffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
			incomplete_multisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
			incomplete_layer_targets = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
		};
	private:

		frame_buffer(glm::ivec2 shape) noexcept;

	public:
		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments);

		frame_buffer(glm::ivec2 shape, no_color_attachment_t, texture2d depth_attachment);

		frame_buffer(glm::ivec2 shape, no_color_attachment_t, render_buffer depth_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, use_depth_stencil_common_attachment_t, texture2d depth_stencil_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, use_depth_stencil_common_attachment_t, render_buffer depth_stencil_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, texture2d depth_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, render_buffer depth_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, texture2d depth_attachment, texture2d stencil_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, texture2d depth_attachment, render_buffer stencil_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, render_buffer depth_attachment, texture2d stencil_attachment);

		frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, render_buffer depth_attachment, render_buffer stencil_attachment);

		/**
		 * @brief 移动@p other 的所有权
		 * @details @p other 被移动后处于有效但未指定的状态, 即调用后`other.status() == tlk::frame_buffer::status_type::unspecified`;
		 * @param other 要移动的frame_buffer
		*/
		frame_buffer(frame_buffer&& other) noexcept;

		~frame_buffer() noexcept;

		 /**
		 * @brief 移动@p other 的所有权
		 * @details @p other 被移动后处于有效但未指定的状态, 即调用后`other.status() == tlk::frame_buffer::status_type::unspecified`;
		 * @param other 要移动的frame_buffer
		 */
		auto operator= (frame_buffer&& other) noexcept ->frame_buffer&;

		auto swap(frame_buffer& other) noexcept ->void;

		friend auto swap(frame_buffer& lhs, frame_buffer& rhs) noexcept ->void {
			lhs.swap(rhs);
		}

		auto transfer_to(detail::default_frame_buffer_&) const ->void;

		auto transfer_to(frame_buffer& other) const ->void;

		[[nodiscard]]
		auto get_id() const noexcept ->uint_t;

		[[nodiscard]]
		auto status() const noexcept ->status_type;

		/**
		 * \brief 复制frame_buffer的颜色缓冲区到另一frame_buffer的颜色缓冲区
		 *
		 * 将该frame_buffer的读缓冲(由`read_from`, 默认为0号颜色附件)的指定区域(由参数src_rect指定)复制到`dest`的绘制缓冲(由`draw_into`设置的颜色附件, 默认为0号颜色附件)的指定区域(由参数`dest_rect`指定)
		 *
		 * \param src_rect 指定颜色附件读取的区域(左上角x坐标, 左上角y坐标, 宽, 高)
		 * \param dst 目标frame_buffer
		 * \param dst_rect 指定dst的颜色附件读取的区域(左上角x坐标, 左上角y坐标, 宽, 高)
		 * \param mask 读取掩码
		 * \param filtering 过滤方式
	     *
		 * \see read_from
		 * \see draw_into
		 */
		auto blit_to(glm::ivec4 src_rect, frame_buffer& dst, glm::ivec4 dst_rect, frame_attachment_mask mask = frame_attachment_mask::all, filtering_policy filtering = filtering_policy::nearest) const ->void;

		/**
		 * \brief 你是\see asa
		 * \param dst
		 * \param dst_rect
		 * \param mask
		 * \param filtering
		 */
		auto blit_to(frame_buffer& dst, glm::ivec4 dst_rect, frame_attachment_mask mask = frame_attachment_mask::all, filtering_policy filtering = filtering_policy::nearest) const ->void;

		auto blit_to(glm::ivec4 src_rect, const detail::default_frame_buffer_& dst, glm::ivec4 dst_rect, frame_attachment_mask mask = frame_attachment_mask::all, filtering_policy filtering = filtering_policy::nearest) const ->void;

		auto blit_to(const detail::default_frame_buffer_& dst, glm::ivec4 dst_rect, frame_attachment_mask mask = frame_attachment_mask::all, filtering_policy filtering = filtering_policy::nearest) const ->void;

		auto clear_color(glm::vec4 color, int_t color_attachment_index = 0)  ->frame_buffer&;

		auto clear_color(glm::vec4 color, int_t color_attachment_index = 0) const ->const frame_buffer&;

		auto clear_color(glm::uvec4 tombstone, int_t color_attachment_index = 0) ->frame_buffer&;

		auto clear_color(glm::uvec4 tombstone, int_t color_attachment_index = 0) const ->const frame_buffer&;

		auto clear_depth(float_t depth = 1.f) ->frame_buffer&;

		auto clear_depth(float_t depth = 1.f) const ->const frame_buffer&;

		auto clear_stencil(int_t stencil = 0) ->frame_buffer&;

		auto clear_stencil(int_t stencil = 0) const ->const frame_buffer&;

		auto shape() const noexcept ->glm::ivec2;

		auto enable(capability cap) ->frame_buffer&;

		auto enable(capability cap) const ->const frame_buffer&;

		auto disable(capability cap) ->frame_buffer&;

		auto disable(capability cap) const ->const frame_buffer&;

		auto bind() const ->void;

		[[nodiscard]]
		auto operator[](int_t index) const ->const texture2d&;

		[[nodiscard]]
		auto color_attachments() const ->const std::vector<texture2d>&;

		[[nodiscard]]
		auto depth_attachment() const ->const std::variant<std::monostate, render_buffer, texture2d>&;

		[[nodiscard]]
		auto stencil_attachment() const ->const std::variant<std::monostate, render_buffer, texture2d>&;

		[[nodiscard]]
		auto detach_color_attachments() ->std::vector<texture2d>;

		auto enable_draw_into(std::initializer_list<color_attachment_t> attatchments) const ->void;

		template<std::ranges::contiguous_range Attachments> requires std::same_as<std::remove_cv_t<std::ranges::range_value_t<Attachments>>, color_attachment_t>
		auto enable_draw_into(Attachments&& attatchments) const ->void requires std::convertible_to<decltype(std::ranges::data(attatchments)), color_attachment_t*> {
			nifo_gl_invoke(glNamedFramebufferDrawBuffers, id, std::ranges::size(attatchments), reinterpret_cast<const GLenum*>(std::ranges::data(attatchments)));
		}

		auto read_from(color_attachment_t attachment) const ->void;

		auto draw_into(color_attachment_t attachment) const ->void;
	private:

		auto check_complete_() const ->void;
	private:
		uint_t id{};
		glm::ivec2 shape_{}; // width and height
		std::vector<texture2d> color_attachments_; // color attachments
		std::variant<std::monostate, render_buffer, texture2d> non_color_attachment_; // 非颜色缓冲附件: 深度缓冲附件或深度缓冲与模板缓冲共用的附件
		std::variant<std::monostate, render_buffer, texture2d> backup_non_color_attachment_; // 备用非颜色缓冲附件: 若深度缓冲和模板缓冲共用则始终处于monostate, 否则是模板缓冲附件
		mutable std::set<capability> caps; // gl capabilities
	};
}
