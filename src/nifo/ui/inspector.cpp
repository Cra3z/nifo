#include <nifo/core/ecs/components/light.h>
#include <nifo/ui/inspector.h>
#include <nifo/ui/hierarchy.h>
#include <nifo/ui/panels/name_panel.h>
#include <nifo/ui/panels/transform_panel.h>
#include <nifo/ui/panels/parallel_light_panel.h>
#include <nifo/ui/panels/point_light_panel.h>
#include <nifo/ui/panels/spot_light_panel.h>

namespace nifo::ui {

	inspector::inspector(hierarchy& hierarchy_, QWidget* parent) :
		QWidget(parent),
		hierarchy_(hierarchy_),
		scroll_area_(new QScrollArea{this}),
		spacer_(new QSpacerItem(40, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding)),
		ctors_{
			{entt::type_id<components::transform>(), +[](QWidget* parent_) ->panel_base* {return new transform_panel{parent_};}},
			{entt::type_id<components::name>(), +[](QWidget* parent_) ->panel_base* {return new name_panel{parent_};}},
			{entt::type_id<components::parallel_light>(), +[](QWidget* parent_) ->panel_base* {return new parallel_light_panel{parent_};}},
			{entt::type_id<components::point_light>(), +[](QWidget* parent_) ->panel_base* {return new point_light_panel{parent_};}},
			{entt::type_id<components::spot_light>(), +[](QWidget* parent_) ->panel_base* {return new spot_light_panel{parent_};}},
		}
	{
		assert(hierarchy_.associated_inspector_ == nullptr); // precondition
		hierarchy_.associated_inspector_ = this;
		auto layout_widget = new QWidget;
		vbox_ = new QVBoxLayout{layout_widget};
		scroll_area_->setWidget(layout_widget);
		scroll_area_->setWidgetResizable(true);
		setLayout(new QVBoxLayout{this});
		layout()->addWidget(scroll_area_);
		vbox_->setAlignment(Qt::AlignTop);
		vbox_->addSpacerItem(spacer_);
	}

	auto inspector::bind(node& node_) ->void {
		auto needed_ = node_.components_type_info();
		std::erase_if(needed_, [this] (const entt::type_info& type_info) {
			return not ctors_.contains(type_info);
		});
		std::vector<entt::type_info> not_needed_;
		for (auto comps_ = comp_panels_ | std::views::keys; const auto& comp_ : comps_) {
			if (std::ranges::find(needed_, comp_) == needed_.end()) not_needed_.push_back(comp_);
		}
		for (const auto& type_ : needed_) {
			(*this)[type_].rebind_node(node_);
			comp_panels_[type_]->show();
		}
		for (const auto& type_ : not_needed_) {
			comp_panels_[type_]->hide();
		}
		vbox_->removeItem(spacer_);
		vbox_->addSpacerItem(spacer_);
	}

	auto inspector::hide_panels() -> void {
		for (auto panel_ : comp_panels_ | std::views::values) panel_->hide();
	}

	auto inspector::operator[] (const entt::type_info& type) -> panel_base& {
		if (not comp_panels_.contains(type)) {
			auto [it, _] = comp_panels_.emplace(type, ctors_.at(type)(this));
			vbox_->addWidget(it->second);
			QObject::connect(it->second, &panel_base::new_value_inputed, this, &inspector::new_value_inputed_in_some_panel);
		}
		return *comp_panels_[type];
	}
}
