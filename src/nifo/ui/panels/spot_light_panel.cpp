#include <QColorDialog>
#include <nifo/ui/panels/spot_light_panel.h>
#include <nifo/ui/panels/spot_light_panel.ui.h>
#include <nifo/core/ecs/components/light.h>
#include <nifo/ui/for_qss.h>

namespace nifo::ui {

    spot_light_panel::spot_light_panel(QWidget* parent) : panel_base(entt::type_id<components::spot_light>(), parent), ui(std::make_unique<Ui::spot_light_panel>()) {
        ui->setupUi(this);
		push_button_->setText("聚光灯");
    	layout()->setAlignment(Qt::AlignTop);
    	layout()->addWidget(ui->layoutWidget);

    	ui->outer_cutoff->set_range(15, 165);

    	set_usage(ui->color, "color-selector");

    	auto color_dialog = new QColorDialog{this};

    	connect(this, &panel_base::subwidgets_reload_requested, [this, color_dialog] {
    		auto light_ = static_cast<components::spot_light*>(component_);
			ui->diffuse_strength->setValue(int(light_->diffuse * 100.f));
			ui->specular_strength->setValue(int(light_->specular * 100.f));
    		ui->outer_cutoff->set_value(light_->outer_cutoff);
    		ui->inner_div_outer->setValue(int(light_->inner_div_outer * 100.f));
    		ui->color->setStyleSheet(QString::fromStdString(fmt::format(
    			"background-color: rgba({}, {}, {}, {})", 255.f * light_->color.r, 255.f * light_->color.g, 255.f * light_->color.b, 255.f * light_->color.a)
    		));
    		color_dialog->setCurrentColor(QColor(255.f * light_->color.r, 255.f * light_->color.g, 255.f * light_->color.b, 255.f * light_->color.a));
		});

    	connect(ui->diffuse_strength, &QSlider::valueChanged, [this] (int value) {
    		auto light_ = static_cast<components::spot_light*>(component_);
    		light_->diffuse = float(value) / 100.f;
    		this->propagate_new_value_inputed();
    	});

    	connect(ui->specular_strength, &QSlider::valueChanged, [this] (int value) {
    		auto light_ = static_cast<components::spot_light*>(component_);
    		light_->specular = float(value) / 100.f;
    		this->propagate_new_value_inputed();
    	});

    	connect(ui->outer_cutoff, &floating_point_spinbox::value_changed, [this] (float value) {
			auto light_ = static_cast<components::spot_light*>(component_);
			light_->outer_cutoff = float(value);
			this->propagate_new_value_inputed();
		});

    	connect(ui->inner_div_outer, &QSlider::valueChanged, [this] (int value) {
			auto light_ = static_cast<components::spot_light*>(component_);
			light_->inner_div_outer = float(value) / 100.f;
			this->propagate_new_value_inputed();
		});

    	connect(ui->color, &QPushButton::clicked, [this, color_dialog] {
    		auto light_ = static_cast<components::spot_light*>(component_);
			color_dialog->exec();
    		auto new_color = color_dialog->currentColor();
    		auto color = glm::vec4(new_color.red(), new_color.green(), new_color.blue(), new_color.alpha());
    		light_->color = color / 255.f;
    		ui->color->setStyleSheet(QString::fromStdString(fmt::format(
    			"background-color: rgba({}, {}, {}, {})", color.r, color.g, color.b, color.a
    		)));
    		this->propagate_new_value_inputed();
    	});
    }

    auto spot_light_panel::rebind_node(node& node_) -> void {
    	rebind_comp_(node_.get_components<components::spot_light>());
    }

    auto spot_light_panel::do_expand() -> void {
    	ui->layoutWidget->show();
    }

    auto spot_light_panel::do_collapse() -> void {
    	ui->layoutWidget->hide();
    }

    spot_light_panel::~spot_light_panel() = default;

} // nifo::ui
