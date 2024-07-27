#include <QHBoxLayout>
#include <nifo/ui/widgets/import_3dmodel_preset.h>
#include <nifo/ui/widgets/import_3dmodel_preset.ui.h>
#include <nifo/ui/main_window.h>

namespace nifo::ui {

    import_3dmodel_preset::import_3dmodel_preset(QWidget* parent) : QWidget(parent), ui(std::make_unique<Ui::import_3dmodel_preset>()) {
    	ui->setupUi(this);
    	ui->scale->set_value(1.f);
    }
    
    import_3dmodel_preset::~import_3dmodel_preset() = default;

    auto import_3dmodel_preset::get_preset_info() const -> detail::model_config {
    	return {ui->forward_axis->currentIndex(), ui->up_axis->currentIndex(), float(ui->scale->value()), ui->flip_uv->isChecked()};
    }
} // nifo::ui
