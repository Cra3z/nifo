#pragma once
#include <QPushButton>
#include <QVBoxLayout>
#include <entt/entt.hpp>
#include "../../core/ecs/components/common.h"
#include "../../core/utils/core.h"
#include "../../core/scene.h"

namespace nifo::ui {
	class panel_base : public QWidget {
		friend class input_base;
		Q_OBJECT
	public:
		explicit panel_base(entt::type_info type, QWidget* parent = nullptr);

		virtual auto rebind_node(node& node_) ->void = 0;

	protected:

		template<nifo::component T>
		auto rebind_comp_(T& component) ->void {
			assert(entt::type_id<T>() == type_);
			emit rebind_requested(static_cast<void*>(std::addressof(component)), entt::type_id<T>());
		}

	public slots:

		auto request_subwidgets_reload(void* component, [[maybe_unused]] entt::type_info component_type) ->void ;

		auto expand() ->void ;

		auto collapse() ->void ;

		auto toggle() ->void ;

		auto propagate_new_value_inputed() ->void ;

	private:

		virtual auto do_expand() ->void = 0;

		virtual auto do_collapse() ->void = 0;

		virtual auto on_new_value_inputed() ->void {}

	signals:

		auto rebind_requested(void* component, entt::type_info component_type) ->void;

		auto subwidgets_reload_requested() ->void;

		auto new_value_inputed() ->void;

	protected:
		void* component_ = nullptr;
		entt::type_info type_;
		QPushButton* push_button_;
		bool expanded;
	};
}