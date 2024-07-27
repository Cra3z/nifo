#include <QStyle>
#include <QApplication>
#include <nifo/ui/panels/common.h>
#include <nifo/ui/for_qss.h>

namespace nifo::ui {
	panel_base::panel_base(entt::type_info type, QWidget* parent) : QWidget(parent), type_(type), push_button_(new QPushButton{this}), expanded(true) {
		setLayout(new QVBoxLayout{this});
		set_usage(push_button_, "panel-title");
		set_qss_property(push_button_, "associated-panel-expanded", true);
		layout()->addWidget(push_button_);
		QObject::connect(this, &panel_base::rebind_requested, this, &panel_base::request_subwidgets_reload);
		QObject::connect(push_button_, &QPushButton::clicked, this, &panel_base::toggle);
	}

	auto panel_base::request_subwidgets_reload(void* component, entt::type_info component_type) -> void {
		assert(type_ == component_type);
		component_ = component;
		emit subwidgets_reload_requested();
	}

	auto panel_base::expand() -> void {
		do_expand();
		expanded = true;
		set_qss_property(push_button_, "associated-panel-expanded", true);
	}

	auto panel_base::collapse() -> void {
		do_collapse();
		expanded = false;
		set_qss_property(push_button_, "associated-panel-expanded", false);
	}

	auto panel_base::toggle() -> void {
		if (expanded) collapse();
		else expand();
	}

	auto panel_base::propagate_new_value_inputed() -> void {
		on_new_value_inputed();
		emit new_value_inputed();
	}
}
