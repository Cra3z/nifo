#include <nifo/core/ogl/camera.h>
#include <nifo/core/ogl/frame_buffer.h>

namespace nifo {

	namespace {
		class scoped_bind_frame_buffer {
		public:
			explicit scoped_bind_frame_buffer(uint_t id)  {
				nifo_gl_invoke(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &prev_id);
				nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, id);
			}

			scoped_bind_frame_buffer(const scoped_bind_frame_buffer&) = delete;

			auto operator= (const scoped_bind_frame_buffer&) ->scoped_bind_frame_buffer& = delete;

			~scoped_bind_frame_buffer() noexcept {
				nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, prev_id);
			}
		private:
			int_t prev_id{};
		};
	}

	render_buffer::render_buffer(uint_t id) noexcept: id(id) {}

	render_buffer::render_buffer(render_buffer&& other) noexcept: id(std::exchange(other.id, {})) {}

	render_buffer::~render_buffer() noexcept {
		nifo_gl_invoke(glDeleteRenderbuffers, 1, &id);
	}

	auto render_buffer::operator=(render_buffer&& other) noexcept -> render_buffer& {
		render_buffer tmp = std::move(other);
		swap(tmp);
		return *this;
	}

	auto render_buffer::swap(render_buffer& other) noexcept -> void {
		std::ranges::swap(id, other.id);
	}

	auto render_buffer::bind() const -> void {
		nifo_gl_invoke(glBindRenderbuffer, GL_RENDERBUFFER, id);
	}

	auto render_buffer::unbind() const -> void {
		nifo_gl_invoke(glBindRenderbuffer, GL_RENDERBUFFER, 0);
	}

	auto render_buffer::set_storage(internalformat ifmt, ssize_t width, ssize_t height, ssize_t multi_sample) -> void {
		nifo_gl_invoke(glNamedRenderbufferStorageMultisample, id, multi_sample, to_underlying(ifmt), width, height);
	}

	auto render_buffer::get_id() const noexcept -> uint_t {
		return id;
	}

	auto create_render_buffer(
		render_buffer::internalformat internalformat, ssize_t width, ssize_t height, ssize_t multi_sample
	) -> render_buffer {
		uint_t id;
		nifo_gl_invoke(glCreateRenderbuffers, 1, &id);
		render_buffer result{id};
		result.set_storage(internalformat, width, height, multi_sample);
		return result;
	}

	detail::default_frame_buffer_::default_frame_buffer_(uint_t id) : id(id) {}

	auto detail::default_frame_buffer_::transfer_to(frame_buffer& other) ->void {
		auto& caps = info[id];
		if (other.status() == nifo::frame_buffer::status_type::unspecified) throw runtime_error{"frame buffer is unspecified"};
		for (auto& cap : caps) nifo_gl_invoke(glDisable, to_underlying(cap));
		nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, other.id);
		for (auto& cap : other.caps) nifo_gl_invoke(glEnable, to_underlying(cap));
	}

	auto detail::default_frame_buffer_::enable(capability cap) -> default_frame_buffer_& {
		auto& caps = info[id];
		[[maybe_unused]] scoped_bind_frame_buffer s_{id};
		nifo_gl_invoke(glEnable, to_underlying(cap));
		caps.insert(cap);
		return *this;
	}

	auto detail::default_frame_buffer_::disable(capability cap) -> default_frame_buffer_& {
		auto& caps = info[id];
		[[maybe_unused]] scoped_bind_frame_buffer s_{id};
		caps.erase(cap);
		nifo_gl_invoke(glDisable, to_underlying(cap));
		return *this;
	}

	auto detail::default_frame_buffer_::clear_color(glm::vec4 color, int_t color_attachment_index) ->detail::default_frame_buffer_& {
		nifo_gl_invoke(glClearNamedFramebufferfv, id, GL_COLOR, color_attachment_index, glm::value_ptr(color));
		return *this;
	}

	auto detail::default_frame_buffer_::clear_depth(float_t depth) ->detail::default_frame_buffer_& {
		nifo_gl_invoke(glClearNamedFramebufferfv, id, GL_DEPTH, 0, &depth);
		return *this;
	}

	auto detail::default_frame_buffer_::clear_stencil(int_t stencil) -> detail::default_frame_buffer_& {
		nifo_gl_invoke(glClearNamedFramebufferiv, id, GL_STENCIL, 0, &stencil);
		return *this;
	}

	auto detail::default_frame_buffer_::set_reading_attachment(default_frame_buffer_source attachment) ->void {
		nifo_gl_invoke(glNamedFramebufferReadBuffer, id, to_underlying(attachment));
	}

	auto detail::default_frame_buffer_::set_drawing_attachment(default_frame_buffer_source attachment) -> void {
		nifo_gl_invoke(glNamedFramebufferDrawBuffer, id, to_underlying(attachment));
	}

	auto detail::default_frame_buffer_::bind() const -> void {
		nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, id);
	}

	frame_buffer::frame_buffer(glm::ivec2 shape) noexcept : shape_(shape) {
		nifo_gl_invoke(glCreateFramebuffers, 1, &id);
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments) : frame_buffer(shape) {
		color_attachments_ = std::move(color_attachments);
		for (int_t i = 0; auto& col_attachment : color_attachments_) {
			nifo_gl_invoke(glNamedFramebufferTexture, id, GL_COLOR_ATTACHMENT0 + i, col_attachment.get_id(), 0);
			++i;
		}
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, no_color_attachment_t, texture2d depth_attachment) : frame_buffer(shape) {
		nifo_gl_invoke(glNamedFramebufferDrawBuffer, id, GL_NONE);
		nifo_gl_invoke(glNamedFramebufferReadBuffer, id, GL_NONE);
		non_color_attachment_ = std::move(depth_attachment);
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_DEPTH_ATTACHMENT, std::get<texture2d>(non_color_attachment_).get_id(), 0);
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, no_color_attachment_t, render_buffer depth_attachment) : frame_buffer(shape) {
		nifo_gl_invoke(glNamedFramebufferDrawBuffer, id, GL_NONE);
		nifo_gl_invoke(glNamedFramebufferReadBuffer, id, GL_NONE);
		non_color_attachment_ = std::move(depth_attachment);
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, std::get<render_buffer>(non_color_attachment_).get_id());
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, use_depth_stencil_common_attachment_t, texture2d depth_stencil_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_stencil_attachment);
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_DEPTH_STENCIL_ATTACHMENT, std::get<texture2d>(non_color_attachment_).get_id(), 0);
		check_complete_();
	}
	
	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, use_depth_stencil_common_attachment_t, render_buffer depth_stencil_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_stencil_attachment);
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, std::get<render_buffer>(non_color_attachment_).get_id());
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, texture2d depth_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_attachment);
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_DEPTH_ATTACHMENT, std::get<texture2d>(non_color_attachment_).get_id(), 0);
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, render_buffer depth_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_attachment);
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, std::get<render_buffer>(non_color_attachment_).get_id());
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, texture2d depth_attachment, texture2d stencil_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_attachment);
		backup_non_color_attachment_ = std::move(stencil_attachment);
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_DEPTH_ATTACHMENT, std::get<texture2d>(non_color_attachment_).get_id(), 0);
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_STENCIL_ATTACHMENT, std::get<texture2d>(backup_non_color_attachment_).get_id(), 0);
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, texture2d depth_attachment, render_buffer stencil_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_attachment);
		backup_non_color_attachment_ = std::move(stencil_attachment);
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_DEPTH_ATTACHMENT, std::get<texture2d>(non_color_attachment_).get_id(), 0);
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, std::get<render_buffer>(backup_non_color_attachment_).get_id());
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, render_buffer depth_attachment, texture2d stencil_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_attachment);
		backup_non_color_attachment_ = std::move(stencil_attachment);
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, std::get<render_buffer>(non_color_attachment_).get_id());
		nifo_gl_invoke(glNamedFramebufferTexture, id, GL_STENCIL_ATTACHMENT, std::get<texture2d>(backup_non_color_attachment_).get_id(), 0);
		check_complete_();
	}

	frame_buffer::frame_buffer(glm::ivec2 shape, std::vector<texture2d> color_attachments, render_buffer depth_attachment, render_buffer stencil_attachment) :
		frame_buffer(shape, std::move(color_attachments)) {
		non_color_attachment_ = std::move(depth_attachment);
		backup_non_color_attachment_ = std::move(stencil_attachment);
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_DEPTH_ATTACHMENT,   GL_RENDERBUFFER, std::get<render_buffer>(non_color_attachment_).get_id());
		nifo_gl_invoke(glNamedFramebufferRenderbuffer, id, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, std::get<render_buffer>(backup_non_color_attachment_).get_id());
		check_complete_();
	}

	frame_buffer::frame_buffer(frame_buffer&& other) noexcept :
		id(std::exchange(other.id, {})), shape_(std::exchange(other.shape_, {})),
		color_attachments_(std::move(other.color_attachments_)),
		non_color_attachment_(std::move(other.non_color_attachment_)),
		backup_non_color_attachment_(std::move(other.backup_non_color_attachment_)),
		caps(std::move(other.caps)) {}

	frame_buffer::~frame_buffer() noexcept {
		nifo_gl_invoke(glDeleteFramebuffers, 1, &id);
	}

	auto frame_buffer::operator=(frame_buffer&& other) noexcept -> frame_buffer& {
		frame_buffer tmp = std::move(other);
		swap(tmp);
		return *this;
	}

	auto frame_buffer::swap(frame_buffer& other) noexcept -> void {
		std::ranges::swap(id, other.id);
	}

	auto frame_buffer::transfer_to(detail::default_frame_buffer_& default_buffer) const ->void {
		for (auto& cap : caps) nifo_gl_invoke(glDisable, to_underlying(cap));
		nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, default_buffer.id);
		for (auto& cap : detail::default_frame_buffer_::info[default_buffer.id]) nifo_gl_invoke(glEnable, to_underlying(cap));
	}

	auto frame_buffer::transfer_to(frame_buffer& other) const ->void {
		if (other.status() == nifo::frame_buffer::status_type::unspecified) throw runtime_error{"frame buffer is unspecified"};
		for (auto& cap : caps) nifo_gl_invoke(glDisable, to_underlying(cap));
		nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, other.id);
		for (auto& cap : other.caps) nifo_gl_invoke(glEnable, to_underlying(cap));
	}

	auto frame_buffer::get_id() const noexcept -> uint_t {
		return id;
	}

	auto frame_buffer::status() const noexcept ->status_type {
		return id == 0 ? status_type::unspecified : static_cast<status_type>(nifo_gl_invoke(glCheckNamedFramebufferStatus, id, GL_FRAMEBUFFER));
	}

	auto frame_buffer::blit_to(
		glm::ivec4 src_rect, frame_buffer& dst, glm::ivec4 dst_rect, frame_attachment_mask mask, filtering_policy filtering
	) const -> void {
		nifo_gl_invoke(
			glBlitNamedFramebuffer,
			id, dst.id,
			src_rect[0], src_rect[1], src_rect[0] + src_rect[2], src_rect[1] + src_rect[3],
			dst_rect[0], dst_rect[1], dst_rect[0] + dst_rect[2], dst_rect[1] + dst_rect[3],
			to_underlying(mask), to_underlying(filtering)
		);
	}

	auto frame_buffer::blit_to(
		frame_buffer& dst, glm::ivec4 dst_rect, frame_attachment_mask mask, filtering_policy filtering
	) const -> void {
		blit_to({0, 0, shape_[0], shape_[1]}, dst, dst_rect, mask, filtering);
	}

	auto frame_buffer::blit_to(
		glm::ivec4 src_rect, const detail::default_frame_buffer_& dst, glm::ivec4 dst_rect, frame_attachment_mask mask,
		filtering_policy filtering
	) const -> void {
		nifo_gl_invoke(
			glBlitNamedFramebuffer,
			id, dst.id,
			src_rect[0], src_rect[1], src_rect[0] + src_rect[2], src_rect[1] + src_rect[3],
			dst_rect[0], dst_rect[1], dst_rect[0] + dst_rect[2], dst_rect[1] + dst_rect[3],
			to_underlying(mask), to_underlying(filtering)
		);
	}

	auto frame_buffer::blit_to(
		const detail::default_frame_buffer_& dst, glm::ivec4 dst_rect, frame_attachment_mask mask, filtering_policy filtering
	) const -> void {
		blit_to({0, 0, shape_[0], shape_[1]}, dst, dst_rect, mask, filtering);
	}

	auto frame_buffer::clear_color(glm::vec4 color, int_t color_attachment_index) ->frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferfv, id, GL_COLOR, color_attachment_index, glm::value_ptr(color));
		return *this;
	}

	auto frame_buffer::clear_color(glm::vec4 color, int_t color_attachment_index) const ->const frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferfv, id, GL_COLOR, color_attachment_index, glm::value_ptr(color));
		return *this;
	}

	auto frame_buffer::clear_color(glm::uvec4 tombstone, int_t color_attachment_index) -> frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferuiv, id, GL_COLOR, color_attachment_index, glm::value_ptr(tombstone));
		return *this;
	}

	auto frame_buffer::clear_color(glm::uvec4 tombstone, int_t color_attachment_index) const -> const frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferuiv, id, GL_COLOR, color_attachment_index, glm::value_ptr(tombstone));
		return *this;
	}

	auto frame_buffer::clear_depth(float_t depth) ->frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferfv, id, GL_DEPTH, 0, &depth);
		return *this;
	}

	auto frame_buffer::clear_depth(float_t depth) const ->const frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferfv, id, GL_DEPTH, 0, &depth);
		return *this;
	}

	auto frame_buffer::clear_stencil(int_t stencil) ->frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferiv, id, GL_STENCIL, 0, &stencil);
		return *this;
	}

	auto frame_buffer::clear_stencil(int_t stencil) const ->const frame_buffer& {
		nifo_gl_invoke(glClearNamedFramebufferiv, id, GL_STENCIL, 0, &stencil);
		return *this;
	}

	auto frame_buffer::shape() const noexcept -> glm::ivec2 {
		return shape_;
	}

	auto frame_buffer::enable(capability cap) ->frame_buffer& {
		[[maybe_unused]] scoped_bind_frame_buffer s_{id};
		nifo_gl_invoke(glEnable, to_underlying(cap));
		caps.insert(cap);
		return *this;
	}

	auto frame_buffer::enable(capability cap) const ->const frame_buffer& {
		[[maybe_unused]] scoped_bind_frame_buffer s_{id};
		nifo_gl_invoke(glEnable, to_underlying(cap));
		caps.insert(cap);
		return *this;
	}

	auto frame_buffer::disable(capability cap) ->frame_buffer& {
		[[maybe_unused]] scoped_bind_frame_buffer s_{id};
		caps.erase(cap);
		nifo_gl_invoke(glDisable, to_underlying(cap));
		return *this;
	}

	auto frame_buffer::disable(capability cap) const ->const frame_buffer& {
		[[maybe_unused]] scoped_bind_frame_buffer s_{id};
		caps.erase(cap);
		nifo_gl_invoke(glDisable, to_underlying(cap));
		return *this;
	}

	auto frame_buffer::bind() const -> void {
		nifo_gl_invoke(glBindFramebuffer, GL_FRAMEBUFFER, id);
	}

	auto frame_buffer::operator[] (int_t index) const ->const texture2d& {
		return color_attachments_.at(index);
	}

	auto frame_buffer::color_attachments() const ->const std::vector<texture2d>& {
		return color_attachments_;
	}

	auto frame_buffer::depth_attachment() const ->const std::variant<std::monostate, render_buffer, texture2d>& {
		return non_color_attachment_;
	}

	auto frame_buffer::stencil_attachment() const ->const std::variant<std::monostate, render_buffer, texture2d>& {
		return backup_non_color_attachment_;
	}

	auto frame_buffer::detach_color_attachments() ->std::vector<texture2d> {
		return std::move(color_attachments_);
	}

	auto frame_buffer::enable_draw_into(std::initializer_list<color_attachment_t> attatchments) const ->void {
		nifo_gl_invoke(glNamedFramebufferDrawBuffers, id, std::ranges::size(attatchments), attatchments.begin());
	}

	auto frame_buffer::read_from(color_attachment_t attachment) const ->void {
		nifo_gl_invoke(glNamedFramebufferReadBuffer, id, attachment);
	}

	auto frame_buffer::draw_into(color_attachment_t attachment) const ->void {
		nifo_gl_invoke(glNamedFramebufferDrawBuffer, id, attachment);
	}

	auto frame_buffer::check_complete_() const -> void {
		std::string error_info_;
		using enum status_type;
		switch (status()) {
		case complete:
			return;
		case undefined:
			error_info_ = "Undefind";
			break;
		case unspecified:
			error_info_ = "Unspecified";
			break;
		case unsupported:
			error_info_ = "Unsupported";
			break;
		case incomplete_attachment:
			error_info_ = "Incomplete Attachment";
			break;
		case incomplete_missing_attachment:
			error_info_ = "Incomplete Missing Attachment";
			break;
		case incomplete_draw_buffer:
			error_info_ = "Incomplete Draw Buffer";
			break;
		case incomplete_read_buffer:
			error_info_ = "Incomplete Read Buffer";
			break;
		case incomplete_multisample:
			error_info_ = "Incomplete Multisample";
			break;
		case incomplete_layer_targets:
			error_info_ = "Incomplete Layer Targets";
			break;
		}
		throw runtime_error{"the frame buffer incomplete: " + error_info_};
	}
}
