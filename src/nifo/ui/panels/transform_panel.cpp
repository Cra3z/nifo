#include <nifo/ui/panels/transform_panel.h>
#include <nifo/ui/panels/transform_panel.ui.h>

namespace nifo::ui {
	transform_panel::transform_panel(QWidget* parent) : panel_base(entt::type_id<nifo::components::transform>(), parent), ui_(std::make_unique<Ui::transform_panel>()) {
		ui_->setupUi(this);
		push_button_->setText("变换");
		layout()->setAlignment(Qt::AlignTop);
		layout()->addWidget(ui_->layoutWidget);
		const static auto dict = std::vector<std::pair<floating_point_spinbox*, glm::vec3 components::transform::*>>{
			{ui_->position_x, &components::transform::local_position},
			{ui_->position_y, &components::transform::local_position},
			{ui_->position_z, &components::transform::local_position},
			{ui_->rotate_x,   &components::transform::local_rotation},
			{ui_->rotate_y,   &components::transform::local_rotation},
			{ui_->rotate_z,   &components::transform::local_rotation},
			{ui_->scale_x,    &components::transform::local_scale},
			{ui_->scale_y,    &components::transform::local_scale},
			{ui_->scale_z,    &components::transform::local_scale}
		};
		for (std::size_t i{}; auto spinbox_ : dict | std::views::keys) {
			spinbox_->set_single_step(i >= 6 ? .1 : 1.);
			QObject::connect(this, &panel_base::subwidgets_reload_requested, [this, spinbox_, i] {
				spinbox_->set_value((static_cast<components::transform*>(component_)->*(dict.at(i).second))[i % 3]);
			});
			QObject::connect(
				spinbox_, &floating_point_spinbox::value_changed,
				[this, i] (double value) {
					(static_cast<components::transform*>(component_)->*(dict.at(i).second))[i % 3] = float(value);
					this->propagate_new_value_inputed();
				}
			);
			++i;
		}
	}

	auto transform_panel::rebind_node(node& node_) -> void {
		rebind_comp_(node_.get_components<components::transform>());
	}

	auto transform_panel::do_expand() -> void {
		ui_->layoutWidget->show();
	}

	auto transform_panel::do_collapse() -> void {
		ui_->layoutWidget->hide();
	}

	auto transform_panel::on_new_value_inputed() -> void {
		static_cast<components::transform*>(component_)->refresh_cache();
	}

	transform_panel::~transform_panel() = default;
}
