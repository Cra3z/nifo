#pragma once
#include <QDialog>
#include <QTreeView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QFileSystemModel>
#include <QPushButton>
#include "import_3dmodel_preset.h"

namespace nifo {
	struct model_load_information;
}

namespace nifo::ui {
	class import_3dmodel_dialog : public QDialog {
		Q_OBJECT

	public:
		explicit import_3dmodel_dialog(QWidget* parent = nullptr, QStringView title = {}, QStringView filters = {}, QStringView dir = QDir::currentPath());

		[[nodiscard]]
		auto get_selected_models_information() -> std::vector<model_load_information>;

	private slots:
		auto set_filter_(QStringView filter) ->void;

	private:
		QTreeView* file_tree_view;
		QPushButton* ok;
		QPushButton* cancel;
		QComboBox* filter_comboBox;
		QFileSystemModel file_model;
		QStringList selected_files;
		QHBoxLayout* layout_;
		QLineEdit* line_edit_;
		import_3dmodel_preset* preset_;
	};

}