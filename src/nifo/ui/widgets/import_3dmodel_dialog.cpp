#include <spdlog/spdlog.h>
#include <nifo/ui/widgets/import_3dmodel_dialog.h>
#include <nifo/core/ogl/model.h>

namespace nifo::ui {
	import_3dmodel_dialog::import_3dmodel_dialog(QWidget* parent, QStringView title, QStringView filters, QStringView dir):
		QDialog(parent),
		file_tree_view(new QTreeView(this)),
		ok(new QPushButton{"打开", this}),
		cancel(new QPushButton{"取消", this}),
		filter_comboBox(new QComboBox{this}),
		layout_(new QHBoxLayout{this}),
		line_edit_(new QLineEdit{this}),
		preset_(new import_3dmodel_preset{this})
	{
		setWindowTitle(title.toString());
		// setLayout(layout_);
		file_model.setRootPath(dir.toString());
		file_model.setNameFilterDisables(false);
		file_tree_view->setModel(&file_model);
		file_tree_view->setHeaderHidden(true);
		file_tree_view->hideColumn(1);
		file_tree_view->hideColumn(2);
		file_tree_view->hideColumn(3);
		filter_comboBox->addItems(filters.toString().split(";", Qt::SkipEmptyParts));
		file_tree_view->setSortingEnabled(true);
		file_tree_view->sortByColumn(0, Qt::AscendingOrder);
		set_filter_(filter_comboBox->currentText());
		connect(file_tree_view, &QTreeView::doubleClicked, [this] (const QModelIndex& index) {
			if (file_model.isDir(index)) file_tree_view->scrollTo(index);
			else {
				selected_files.clear();
				selected_files.append(file_model.filePath(index));
				accept();
			}
		});
		connect(file_tree_view, &QTreeView::clicked, [&] (const QModelIndex& index) {
			if (not file_model.isDir(index)) line_edit_->setText(file_model.filePath(index));
		});
		connect(filter_comboBox, &QComboBox::currentTextChanged, this, &import_3dmodel_dialog::set_filter_);
		connect(ok, &QPushButton::clicked, [this] {
			selected_files.clear();
			if (not line_edit_->text().isEmpty()) {
				selected_files.append(line_edit_->text());
				accept();
			}
		});
		connect(cancel, &QPushButton::clicked, [this] {
			selected_files.clear();
			reject();
		});
		auto left_side_ = new QWidget{this};
		auto right_side_ = new QWidget{this};
		auto vbox1 = new QVBoxLayout{left_side_};
		vbox1->addWidget(file_tree_view);
		auto hbox1 = new QHBoxLayout{};
		hbox1->addWidget(line_edit_);
		hbox1->addWidget(filter_comboBox);
		vbox1->addLayout(hbox1);
		left_side_->setLayout(vbox1);
		layout_->addWidget(left_side_, 3);

		auto vbox2 = new QVBoxLayout{right_side_};
		vbox2->addWidget(preset_);
		auto hbox2 = new QHBoxLayout{};
		hbox2->addWidget(ok);
		hbox2->addWidget(cancel);
		vbox2->addLayout(hbox2);
		right_side_->setLayout(vbox2);
		layout_->addWidget(right_side_, 2);
		setMinimumSize(720, 450);
		setWindowModality(Qt::WindowModal);
	}

	auto import_3dmodel_dialog::get_selected_models_information() -> std::vector<model_load_information> {
		exec();
		std::vector<model_load_information> result;
		result.reserve(selected_files.size());
		std::ranges::transform(selected_files, std::back_inserter(result), [this] (const QString& file) {
			return model_load_information{preset_->get_preset_info(), file.toStdString()};
		});
		selected_files.clear();
		return result;
	}

	auto import_3dmodel_dialog::set_filter_(QStringView filter) -> void {
		static const QRegularExpression suffix{R"(\((.*\.\w+(\s+.*\.\w+)*)\))"};
		static const QRegularExpression space{R"(\s)"};
		file_model.setNameFilters(suffix.match(filter).captured(1).split(space));
	}
}
