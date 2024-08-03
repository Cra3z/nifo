#pragma once
#include <memory>
#include "common.h"

namespace nifo::ui {

QT_BEGIN_NAMESPACE
    
    namespace Ui {
        class parallel_light_panel; 
    }
    
QT_END_NAMESPACE
    
    class parallel_light_panel : public panel_base {
        Q_OBJECT
    public:
    
        explicit parallel_light_panel(QWidget* parent = nullptr);

		auto rebind_node(node& node_) -> void override;

		auto do_expand() ->void override;

		auto do_collapse() -> void override;

        ~parallel_light_panel() override;
    
    private:
        std::unique_ptr<Ui::parallel_light_panel> ui;
    };

} // nifo::ui
