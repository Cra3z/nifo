#include <QColorDialog>
#include <nifo/ui/panels/point_light_panel.h>
#include <nifo/ui/panels/point_light_panel.ui.h>
#include <nifo/core/ecs/components/light.h>
#include <nifo/ui/for_qss.h>

namespace nifo::ui {

    point_light_panel::point_light_panel(QWidget* parent) : panel_base(entt::type_id<components::point_light>(), parent), ui(std::make_unique<Ui::point_light_panel>()) {
        ui->setupUi(this);
		push_button_->setText("点光源");
    	layout()->setAlignment(Qt::AlignTop);
    	layout()->addWidget(ui->layoutWidget);

    	set_usage(ui->color, "color-selector");

    	auto color_dialog = new QColorDialog{this};

    	connect(this, &panel_base::subwidgets_reload_requested, [this, color_dialog] {
    		auto light_ = static_cast<components::point_light*>(component_);
			ui->power->setValue(int(light_->power));
			ui->diffuse_strength->setValue(int(light_->diffuse * 100.f));
			ui->specular_strength->setValue(int(light_->specular * 100.f));
    		ui->color->setStyleSheet(QString::fromStdString(fmt::format(
    			"background-color: rgba({}, {}, {}, {})", 255.f * light_->color.r, 255.f * light_->color.g, 255.f * light_->color.b, 255.f * light_->color.a)
    		));
    		color_dialog->setCurrentColor(QColor(255.f * light_->color.r, 255.f * light_->color.g, 255.f * light_->color.b, 255.f * light_->color.a));
		});

		connect(ui->power, &QSlider::valueChanged, [this] (int value) {
			auto light_ = static_cast<components::point_light*>(component_);
			light_->power = float(value);
			this->propagate_new_value_inputed();
		});

    	connect(ui->diffuse_strength, &QSlider::valueChanged, [this] (int value) {
    		auto light_ = static_cast<components::point_light*>(component_);
    		light_->diffuse = float(value) / 100.f;
    		this->propagate_new_value_inputed();
    	});

    	connect(ui->specular_strength, &QSlider::valueChanged, [this] (int value) {
    		auto light_ = static_cast<components::point_light*>(component_);
    		light_->specular = float(value) / 100.f;
    		this->propagate_new_value_inputed();
    	});

    	connect(ui->color, &QPushButton::clicked, [this, color_dialog] {
    		auto light_ = static_cast<components::point_light*>(component_);
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

    auto point_light_panel::rebind_node(node& node_) -> void {
    	rebind_comp_(node_.get_components<components::point_light>());
    }

    auto point_light_panel::do_expand() -> void {
    	ui->layoutWidget->show();
    }

    auto point_light_panel::do_collapse() -> void {
    	ui->layoutWidget->hide();
    }

    point_light_panel::~point_light_panel() = default;

} // nifo::ui
