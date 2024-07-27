#pragma once
#include <memory>
#include <QFileDialog>
#include <QWidget>

namespace nifo::detail {
	struct model_config;
}

namespace nifo::ui {

QT_BEGIN_NAMESPACE

    namespace Ui {
        class import_3dmodel_preset;
    }

QT_END_NAMESPACE

    class import_3dmodel_preset : public QWidget {
        Q_OBJECT
    public:

        explicit import_3dmodel_preset(QWidget* parent = nullptr);

        ~import_3dmodel_preset() override;

		[[nodiscard]]
		auto get_preset_info() const ->detail::model_config;
    private:
        std::unique_ptr<Ui::import_3dmodel_preset> ui;
    };

} // nifo::ui
