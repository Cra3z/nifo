#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Compatibility>
#include <nifo/core/ogl/camera.h>
#include <nifo/core/ogl/frame_buffer.h>
#include <nifo/core/ogl/model.h>

namespace nifo::ui {

	class hierarchy;

	class arena : public QOpenGLWidget {
		Q_OBJECT

		friend class hierarchy;

		struct mouse_state;

		struct impl;

	public:

		enum left_button_mouse_behavior : int {
			select,
			translate,
			rotate,
			scale
		};

		enum polygon_mode : int {
			point = GL_POINT,
			line = GL_LINE,
			fill = GL_FILL,
		};

		enum debug_view_mode : int {
			depth,
			normal
		};
	public:

		explicit arena(hierarchy& hierarchy_, QWidget* parent = nullptr);

		~arena() override;

		auto initializeGL() ->void override;

		auto paintGL() ->void override;

		[[nodiscard]]
		auto import_3Dmodel(const model_load_information& preset) -> std::shared_ptr<model>;

	public slots:

		auto set_left_button_behavior(left_button_mouse_behavior behavior) noexcept ->void;

		auto set_view_mode(polygon_mode mode) noexcept ->void;

	private:

		/**
		 * \brief 渲染grid
		 * \pre makeCurrent已调用且gbuffer已绑定
		 */
		auto draw_grid() ->void;

		/**
		 * \brief 渲染相关的hierarchy上的物体
		 * \pre makeCurrent已调用且gbuffer已绑定
		 */
		auto draw_hierarchy() ->void;

		/**
		 * \brief 绘制选择的物体的边框
		 * \pre makeCurrent已调用且gbuffer已绑定
		 */
		auto draw_outlines() ->void;

		/**
		 * \brief 绘制Gizmos
		 * \pre makeCurrent已调用且gbuffer已绑定
		 */
		auto draw_gizmos() ->void;

		/**
		 * \brief 从gbuffer中取出颜色附件i和j合并到默认帧缓冲中
		 * \pre makeCurrent已调用且默认帧缓冲已绑定
		 */
		auto fetch_frame_from_gbuffer(int_t color_attachment_i, int_t color_attachment_j) ->void;

		auto mousePressEvent(QMouseEvent* event) ->void override;

		auto mouseMoveEvent(QMouseEvent* event) ->void override;

		auto mouseReleaseEvent(QMouseEvent* event)->void override;

		auto wheelEvent(QWheelEvent* event) ->void override;

		[[nodiscard]]
		auto default_frame_buffer() const noexcept ->detail::default_frame_buffer_;

		auto select_object_(int x, int y) ->void;

		auto select_object_(entt::entity id) ->void;

		auto delete_object_(entt::entity id) ->void;
	private:
		std::reference_wrapper<hierarchy> hierarchy_;
		std::unique_ptr<impl> pimpl;
	};

	class scoped_make_current {
	public:
		explicit scoped_make_current(arena& arena) noexcept : arena_(arena) {
			arena.makeCurrent();
		}

		scoped_make_current(const scoped_make_current&) = delete;

		~scoped_make_current() {
			arena_.get().doneCurrent();
		}

		auto operator= (const scoped_make_current&) ->scoped_make_current& = delete;

	private:
		std::reference_wrapper<arena> arena_;
	};

}
