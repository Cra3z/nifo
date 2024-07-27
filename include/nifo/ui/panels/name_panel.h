#pragma once
#include <memory>
#include "common.h"

namespace nifo::ui {

    namespace Ui {
        class name_panel;
    }
    
    class name_panel : public panel_base {
        Q_OBJECT
    public:

        explicit name_panel(QWidget* parent = nullptr);

    	auto rebind_node(node& node_) -> void override;

    	auto do_expand() ->void override;

    	auto do_collapse() -> void override;

    	auto on_new_value_inputed() ->void override;

        ~name_panel() override;
    
    private:
        std::unique_ptr<Ui::name_panel> ui_;
    };

}
