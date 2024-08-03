#include <nifo/ui/panels/name_panel.h>
#include <nifo/ui/panels/name_panel.ui.h>
#include <nifo/core/ecs/components/tags.h>

namespace nifo::ui {

    name_panel::name_panel(QWidget* parent) : panel_base(entt::type_id<components::name>(), parent), ui_(std::make_unique<Ui::name_panel>()) {
        ui_->setupUi(this);
    	push_button_->setText("名称");
    	layout()->setAlignment(Qt::AlignTop);
    	layout()->addWidget(ui_->layoutWidget);
    	QObject::connect(this, &panel_base::subwidgets_reload_requested, [this] {
			ui_->name->setText(QString::fromStdString(static_cast<components::name*>(component_)->value));
		});
    	QObject::connect(
			ui_->name, &QLineEdit::editingFinished,
			[this] {
				static_cast<components::name*>(component_)->value = ui_->name->text().toStdString();
				this->propagate_new_value_inputed(next_to_do::refrush_view);
			}
		);
    }

    auto name_panel::rebind_node(node& node_) -> void {
    	rebind_comp_(node_.get_components<components::name>());
    }

    auto name_panel::do_expand() -> void {
		ui_->layoutWidget->show();
    }

    auto name_panel::do_collapse() -> void {
		ui_->layoutWidget->hide();
    }

    auto name_panel::on_new_value_inputed() -> void {}

    name_panel::~name_panel() = default;

}
