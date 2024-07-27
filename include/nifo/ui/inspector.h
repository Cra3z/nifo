#pragma once
#include <QScrollArea>
#include "panels/name_panel.h"
#include "panels/transform_panel.h"

namespace nifo::ui {
	class inspector final : public QWidget {
		Q_OBJECT

		friend class hierarchy;

	public:
		explicit inspector(hierarchy& hierarchy_, QWidget* parent = nullptr);

		[[nodiscard]]
		auto operator[] (const entt::type_info& type) ->panel_base& ;

	signals:

		auto new_value_inputed_in_some_panel() ->void;

	public slots:

		auto bind(nifo::node& node_) ->void;

		auto hide_panels() ->void;
	private:
		std::reference_wrapper<hierarchy> hierarchy_;
		QScrollArea* scroll_area_;
		QVBoxLayout* vbox_;
		QSpacerItem* spacer_;
		std::map<entt::type_info, panel_base*(*)(QWidget*)> ctors_;
		std::map<entt::type_info, panel_base*> comp_panels_;
	};
}
