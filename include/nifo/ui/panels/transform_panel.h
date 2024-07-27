#pragma once
#include <memory>
#include "../../core/ecs/components/transform.h"
#include "common.h"

namespace nifo::ui {

	namespace Ui {
		class transform_panel;
	}

	struct transform_panel final : panel_base {
		Q_OBJECT
	public:
		explicit transform_panel(QWidget* parent = nullptr);

		auto rebind_node(node& node_) -> void override;

		auto do_expand() ->void override;

		auto do_collapse() -> void override;

		auto on_new_value_inputed() ->void override;

		~transform_panel() override;

	private:
		std::unique_ptr<Ui::transform_panel> ui_;
	};
}