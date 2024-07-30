#include <QMouseEvent>
#include <QApplication>
#include <nifo/ui/arena.h>
#include <nifo/core/ogl/frame_buffer.h>
#include <nifo/core/utils/loop.h>
#include <nifo/ui/hierarchy.h>

namespace nifo::ui {

	struct postprocessing_region : mesh {
		explicit postprocessing_region(const nifo::material& default_material) : mesh(
			single_buffer_policy_layout{
				.positions = {{-1.f,  1.f, 0.f}, {-1.f, -1.f, 0.f}, {1.f, -1.f, 0.f}}
			},
			{0, 1, 2},
			1,
			default_material
		) {}
	};

	struct arena::mouse_state {
		left_button_mouse_behavior left_button_behavior = select;

		int pos_x = 0, pos_y = 0;

		float move_speed = 24.f;

		float sensitivity = 0.01f;
	};

	struct arena_context_setter final : context_setter {
		arena_context_setter(arena& arena) noexcept : arena_(arena) {}

		auto make_current() -> void override {
			arena_.get().makeCurrent();
		}

		auto done_current() -> void override {
			arena_.get().doneCurrent();
		}
	private:
		std::reference_wrapper<arena> arena_;
	};

	struct arena::impl {
		arena_context_setter                                context_setter;
		std::unique_ptr<frame_buffer>                       gbuffer;
		entt::entity                                        selected = entt::null;
		mouse_state                                         mouse_state;
		std::map<std::string, material>                     materials;
		texture2d_manager                                   texture2d_manager;
		assets_loader<model, model_load_information>        model_manager;
		assets_loader<shader_part>                          shader_manager;
		std::unique_ptr<postprocessing_region>              postprocessing;
		polygon_mode                                        polygon_mode = fill;
		debug_view_mode                                     debug_view_mode;
		bool                                                update_gbuffer = true;
		QOpenGLFunctions_4_5_Compatibility                  gl_function_caller;

		auto load_default_assets() ->void {
			materials.try_emplace("#grid", material{{.shader = shader_program{
				shader_manager.load(ASSETS_DIR"shaders/builtin/grid.vert"),
				shader_manager.load(ASSETS_DIR"shaders/builtin/grid.frag")
			}}});
			materials.try_emplace("#noop", material{{.shader = shader_program{
				shader_manager.load(ASSETS_DIR"shaders/builtin/noop.vert"),
				shader_manager.load(ASSETS_DIR"shaders/builtin/noop.frag")
			}}});
			materials.try_emplace("#outline", material{{.shader = shader_program{
				shader_manager.load(ASSETS_DIR"shaders/builtin/outline.vert"),
				shader_manager.load(ASSETS_DIR"shaders/builtin/outline.frag")
			}}});
			materials.try_emplace("#merge", material{{.shader = shader_program{
				shader_manager.load(ASSETS_DIR"shaders/builtin/postprocessing/default.vert"),
				shader_manager.load(ASSETS_DIR"shaders/builtin/postprocessing/default.geom"),
				shader_manager.load(ASSETS_DIR"shaders/builtin/postprocessing/merge.frag")
			}}});
		}

	};

	arena::arena(hierarchy& hierarchy_, QWidget* parent) :
		QOpenGLWidget(parent),
		hierarchy_(hierarchy_),
		pimpl(std::make_unique<impl>(*this))
	{
		setMouseTracking(true);
		addAction("delete selected node", QKeySequence::Delete, &hierarchy_, &hierarchy::delete_current_node);
		setContextMenuPolicy(Qt::CustomContextMenu);
		auto menu = new QMenu{};
		menu->addAction(QIcon{":/images/delete-dark.svg"}, "删除节点", &hierarchy_, &hierarchy::delete_current_node);
		connect(this, &QWidget::customContextMenuRequested, menu, [this, menu] (const QPoint& pos) {
			menu->popup(mapToGlobal(pos));
		});
		nifo_expect(hierarchy_.associated_arena_ == nullptr); // precondition
		hierarchy_.associated_arena_ = this;
		detail::caller = &(pimpl->gl_function_caller);
	}

	arena::~arena() {
		scoped_make_current _{*this};
		pimpl.reset();
	}

	auto arena::initializeGL() ->void {
		pimpl->gl_function_caller.initializeOpenGLFunctions();
		auto default_frame_buffer_ = default_frame_buffer();
		default_frame_buffer_.enable(capability::depth_test);
		const auto [fbo_width, fbo_height] = screen()->size() * devicePixelRatio();
		std::vector<texture2d> color_buffers;
		color_buffers.push_back(create_texture2d_storage(fbo_width, fbo_height, texel_format::rgba8));   // 0: for rendering
		color_buffers.push_back(create_texture2d_storage(fbo_width, fbo_height, texel_format::rgba8));   // 1: for selected objects outline
		color_buffers.push_back(create_texture2d_storage(fbo_width, fbo_height, texel_format::grey32u)); // 2: for picking object
		auto depth_and_stencil = create_render_buffer(render_buffer::depth24_stencil8, fbo_width, fbo_height);
		pimpl->load_default_assets();
		pimpl->gbuffer = std::make_unique<frame_buffer>(glm::ivec2{fbo_width, fbo_height}, std::move(color_buffers), use_depth_stencil_common_attachment, std::move(depth_and_stencil));
		pimpl->gbuffer->enable(capability::depth_test).enable_draw_into({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
		pimpl->postprocessing = std::make_unique<postprocessing_region>(pimpl->materials.at("#merge"));
		scene& scene_ = hierarchy_.get().scene_;
		scene_.editor_camera.set_aspect_ratio(float(fbo_width) / float(fbo_height));
		if (scene_.grid == nullptr) scene_.grid = std::make_unique<builtin::grid>(pimpl->materials.at("#grid"), 5.f);
		nifo_gl_invoke(glEnable, GL_DEBUG_OUTPUT);
		nifo_gl_invoke(glEnable, GL_DEBUG_OUTPUT_SYNCHRONOUS);
		nifo_gl_invoke(glDebugMessageCallback, detail::ogl_error_message, nullptr);
	}

	auto arena::paintGL() ->void {
		if (pimpl->update_gbuffer) {
			auto default_frame_buffer_ = default_frame_buffer();
			default_frame_buffer_.clear_color().clear_depth();
			default_frame_buffer_.transfer_to(*pimpl->gbuffer);
			pimpl->gbuffer->
				clear_color(glm::vec4{glm::vec3{0.24}, 1.f}, 0).
				clear_color(glm::vec4{glm::vec3{0.24}, 0.f}, 1).
				clear_color(glm::uvec4{0xff'ff'ff'ff}, 2).
				clear_depth().
				clear_stencil();

			nifo_gl_invoke(glViewport, 0, 0, pimpl->gbuffer->shape().x, pimpl->gbuffer->shape().y);
			draw_grid();
			draw_hierarchy();
			draw_outlines();
			draw_gizmos();
			pimpl->gbuffer->transfer_to(default_frame_buffer_);
		}
		fetch_frame_from_gbuffer(0, 1);
	}

	auto arena::draw_grid() -> void {
		scene& scene_ = hierarchy_.get().scene_;
		const auto view_projection_matrix = scene_.editor_camera.view_projection_matrix();
		scene_.grid->draw([&](const material& mat) {
			mat.shader.set_uniform("view_projection_matrix", view_projection_matrix);
			return primitive::lines;
		});
	}

	auto arena::draw_hierarchy() -> void {
		scene& scene_ = hierarchy_.get().scene_;
		const auto view_projection_matrix = scene_.editor_camera.view_projection_matrix();
		const auto eye = scene_.editor_camera.position();
		if (scene_.root == nullptr) return;
		std::deque<node*> nodes{scene_.root.get()};
		std::ranges::transform(scene_.root->children(), std::back_inserter(nodes), &std::unique_ptr<node>::get);
		while (not nodes.empty()) {
			auto node_ = nodes.front();
			nodes.pop_front();
			if (not node_->has_all_components_of<components::model>() or node_->has_any_component_of<components::hidden>()) continue;
			std::ranges::transform(node_->children(), std::back_inserter(nodes), &std::unique_ptr<node>::get);
			auto& model = node_->get_components<components::model>();
			for (const auto& mesh_ : model.value->meshes()) {
				mesh_.draw([&](const material& mat) {
					node_->transform().cached_.valid_ = false;
					auto model_matrix = node_->transform().local_to_world();
					mat.pass_attributes_to_shader(pimpl->texture2d_manager);
					// optional uniform variables
					void(mat.shader.try_set_uniform("eye", eye));

					// required uniform variables
					mat.shader.set_uniform("MVP", view_projection_matrix * model_matrix);
					mat.shader.set_uniform("object_id", to_underlying(node_->id()));
					mat.shader.set_uniform("normal_matrix", glm::inverse(glm::transpose(glm::mat3{model_matrix})));
					return primitive::triangles;
				});
			}
		}
	}

	auto arena::draw_outlines() -> void {
		if (pimpl->selected != entt::null) {
			scene& scene_ = hierarchy_.get().scene_.get();
			node& selected = scene_.entities.at(pimpl->selected);
			auto MVP = hierarchy_.get().scene_.get().editor_camera.view_projection_matrix() * selected.transform().local_to_world();
			pimpl->gbuffer->enable(capability::stencil_test).enable(capability::color_blending).disable(capability::depth_test);
			scope_exit _{[&] {
				pimpl->gbuffer->disable(capability::stencil_test).disable(capability::color_blending).enable(capability::depth_test);
			}};
			nifo_gl_invoke(glStencilOp, GL_KEEP, GL_KEEP, GL_REPLACE);

			nifo_gl_invoke(glBlendFunc, GL_ZERO, GL_ONE);
			nifo_gl_invoke(glStencilFunc, GL_ALWAYS, 1, 0xff);
			for (const auto& mesh : selected.get_components<components::model>().value->meshes()) {
				mesh.draw_with(pimpl->materials.at("#noop"), [&](const material& material) {
					material.shader.set_uniform("MVP", MVP);
					return primitive::triangles;
				});
			}

			nifo_gl_invoke(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			nifo_gl_invoke(glStencilFunc, GL_NOTEQUAL, 1, 0xff);
			for (const auto& mesh : selected.get_components<components::model>().value->meshes()) {
				mesh.draw_with(pimpl->materials.at("#outline"), [&](const material& material) {
					material.shader.set_uniform("MVP", MVP);
					return primitive::triangles;
				});
			}
			nifo_gl_invoke(glBlendFunc, GL_ONE, GL_ZERO);
		}
	}

	auto arena::draw_gizmos() -> void {} // TODO: 待实现

	auto arena::fetch_frame_from_gbuffer(int_t color_attachment_i, int_t color_attachment_j) -> void {
		nifo_gl_invoke(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
		scope_exit _{[this] {nifo_gl_invoke(glPolygonMode, GL_FRONT_AND_BACK, to_underlying(pimpl->polygon_mode));}};
		const auto [arena_w, arena_h] = size() * devicePixelRatio();
		const glm::vec2 gbuffer_shape = pimpl->gbuffer->shape();
		auto pixel_pos = gbuffer_shape / 2.f;
		pixel_pos.x -= float(arena_w) / 2.f;
		pixel_pos.y -= float(arena_h) / 2.f;
		// pimpl->gbuffer->blit_to(glm::ivec4{pixel_pos.x, pixel_pos.y, arena_w, arena_h}, default_frame_buffer_, {0, 0, arena_w, arena_h}, frame_attachment_mask::all);
		nifo_gl_invoke(glViewport, 0, 0, arena_w, arena_h);
		pimpl->postprocessing->draw([&] (const material& material) {
			(*pimpl->gbuffer)[color_attachment_i].bind_unit(0);
			(*pimpl->gbuffer)[color_attachment_j].bind_unit(1);
			material.shader.set_uniform("rect", glm::vec4{
				pixel_pos.x / gbuffer_shape.x,
				pixel_pos.y / gbuffer_shape.y,
				arena_w / gbuffer_shape.x,
				arena_h / gbuffer_shape.y
			});
			material.shader.set_uniform("src1", 0);
			material.shader.set_uniform("src2", 1);
			return primitive::triangles;
		});
	}

	auto arena::import_3Dmodel(const model_load_information& preset) -> std::shared_ptr<model> {
		return pimpl->model_manager.load(preset, pimpl->shader_manager, pimpl->materials, pimpl->context_setter);
	}

	auto arena::set_left_button_behavior(left_button_mouse_behavior behavior) noexcept -> void {
		pimpl->mouse_state.left_button_behavior = behavior;
	}

	auto arena::set_view_mode(polygon_mode mode) noexcept -> void {
		if (pimpl->polygon_mode == mode) return;
		pimpl->polygon_mode = mode;
		{
			scoped_make_current _{*this};
			switch (mode) {
			case fill:
				nifo_gl_invoke(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
				break;
			case line:
				nifo_gl_invoke(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE);
				break;
			case point:
				nifo_gl_invoke(glPolygonMode, GL_FRONT_AND_BACK, GL_POINT);
				break;
			}

		}
		repaint();
	}

	auto arena::mousePressEvent(QMouseEvent* event) ->void {
		const auto pressed_button = event->buttons();
		const auto [x, y] = mapToGlobal(event->pos());
		if (pressed_button & Qt::MiddleButton) {
			pimpl->mouse_state.pos_x = x;
			pimpl->mouse_state.pos_y = y;
		}
		else if (pressed_button & Qt::LeftButton) {
			nifo_expect(pimpl->gbuffer);
			const auto gbuffer_center = pimpl->gbuffer->shape() / 2;
			const auto [pixel_x, pixel_y] = event->pos() * devicePixelRatio();
			const auto arena_half_width = width() * devicePixelRatio() / 2;
			const auto arena_half_height = height() * devicePixelRatio() / 2;
			switch (pimpl->mouse_state.left_button_behavior) {
			case select:
				select_object_(gbuffer_center.x - (arena_half_width - pixel_x), gbuffer_center.y + (arena_half_height - pixel_y)); // NOLINT
				break;
			case translate:
				// TODO: Transformation Gizmo
				break;
			case scale:
				// TODO: Scaling Gizmo
				break;
			case rotate:
				// TODO: Rotation Gizmo
				break;
			}
		}
		QOpenGLWidget::mousePressEvent(event);
	}

	auto arena::mouseMoveEvent(QMouseEvent* event) ->void {
		auto& editor_camera = hierarchy_.get().scene_.get().editor_camera;
		auto [x, y] = mapToGlobal(event->pos());
		auto pressed_button = event->buttons();
		if (pressed_button & Qt::MiddleButton) {
			scene& scene_ = hierarchy_.get().scene_;
			auto delta = pimpl->mouse_state.sensitivity * glm::vec2(glm::sign(float(pimpl->mouse_state.pos_x - x)), glm::sign(float(pimpl->mouse_state.pos_y - y)));
			switch (QApplication::keyboardModifiers()) {
			case Qt::ShiftModifier:
				editor_camera.move_by(pimpl->mouse_state.move_speed * (delta.x * editor_camera.right() - delta.y * editor_camera.up()));
				break;
			case Qt::ControlModifier:
				editor_camera.drift(delta);
				break;
			default:
				glm::vec3 center = pimpl->selected != entt::null ? scene_.entities.at(pimpl->selected).get().transform().global_position() : glm::vec3{};
				editor_camera.orbit(center, delta);
			}
			pimpl->mouse_state.pos_x = x;
			pimpl->mouse_state.pos_y = y;
			repaint();
		}
		QOpenGLWidget::mouseMoveEvent(event);
	}

	auto arena::mouseReleaseEvent(QMouseEvent* event) ->void {
		QOpenGLWidget::mouseReleaseEvent(event);
	}

	auto arena::wheelEvent(QWheelEvent* event) ->void {
		auto& editor_camera = hierarchy_.get().scene_.get().editor_camera;
		auto zoomed = event->angleDelta().y();
		constexpr float delta = glm::radians(5.f);
		editor_camera.zoom(zoomed > 0 ? -delta : delta);
		repaint();
		QOpenGLWidget::wheelEvent(event);
	}

	auto arena::default_frame_buffer() const noexcept -> detail::default_frame_buffer_ {
		return  detail::default_frame_buffer_ {defaultFramebufferObject()};
	}

	auto arena::select_object_(int x, int y) -> void {
		uint selected_object_id;
		{
			scoped_make_current _{*this};
			default_frame_buffer().transfer_to(*pimpl->gbuffer);
			pimpl->gbuffer->read_from(GL_COLOR_ATTACHMENT2);
			nifo_gl_invoke(glReadPixels, x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &selected_object_id);
			pimpl->gbuffer->read_from(GL_COLOR_ATTACHMENT0);
		}
		scene& scene_ = hierarchy_.get().scene_;
		if (selected_object_id != entt::null) {
			if (auto it = scene_.entities.find(static_cast<entt::entity>(selected_object_id));
				it != scene_.entities.end()
			) {
				hierarchy_.get().select(it->second);
			}
		}
	}

	auto arena::select_object_(entt::entity id) ->void {
		pimpl->selected = id;
	}

	auto arena::delete_object_(entt::entity id) -> void {
		if (pimpl->selected == id) pimpl->selected = entt::null;
	}
}
