#include <deque>
#include <QHeaderView>
#include <QScreen>
#include <nifo/core/utils/any_ref.h>
#include <nifo/ui/hierarchy.h>
#include <nifo/ui/arena.h>
#include <nifo/ui/inspector.h>
#include <nifo/ui/for_qss.h>

namespace nifo::ui {

    hierarchy::hierarchy(scene& scene, QWidget* parent) :
		QWidget(parent),
		scene_(scene),
		menu_(new QMenu{this}),
		model_(build_tree_model_(scene.root->children())),
		tree_view_(new QTreeView{this}),
		model_import_dialog_(std::make_unique<import_3dmodel_dialog>(nullptr, QStringLiteral("打开3D模型"), QStringLiteral("3D模型文件(*.obj *.fbx)")))
	{
    	tree_view_->setModel(model_);
    	model_->setHorizontalHeaderLabels(QStringList{QString::fromStdString(std::format("{}", scene.name))});
    	auto sub_menu = menu_->addMenu("添加节点");
    	set_usage(sub_menu, "import-3Dmodel");
    	sub_menu->addAction("导入模型到当前节点", this, &hierarchy::import_3Dmodel_to_current_node);
    	sub_menu->addAction("导入模型到当前场景", this, &hierarchy::import_3Dmodel_to_current_scene);
    	menu_->addSeparator();
    	menu_->addAction(QIcon{":/images/delete-dark.svg"}, "删除节点", this, &hierarchy::delete_current_node);
    	setContextMenuPolicy(Qt::CustomContextMenu);
    	connect(this, &QWidget::customContextMenuRequested, menu_, [this](const QPoint& pos) {
	        menu_->popup(mapToGlobal(pos));
        });
    	connect(tree_view_->selectionModel(), &QItemSelectionModel::currentChanged, [this] (const QModelIndex& current, [[maybe_unused]] const QModelIndex& previous) {
    		if (current.isValid()) emit current_changed(current.data(Qt::UserRole).value<any_ref>().as<nifo::node>());
    		else emit no_selected();
    	});
    	connect(this, &hierarchy::current_changed, [this] (node& node_) {
    		nifo_expect(associated_arena_ != nullptr);
    		associated_arena_->select_object_(node_.id());
    	});
    }

    auto hierarchy::resizeEvent(QResizeEvent* event) ->void {
	    QWidget::resizeEvent(event);
    	tree_view_->resize(size());
    }

    auto hierarchy::import_3Dmodel_to_current_scene() -> void {
    	nifo_expect(associated_arena_); // precondition

    	scoped_make_current _{*associated_arena_};
    	for (const auto& presets = model_import_dialog_->get_selected_models_information(); const auto& preset : presets) {
    		emit node_importing_started(QString::fromStdString(preset.file.string()));
    		auto new_node = new node{associated_arena_->import_3Dmodel(preset), preset.file.stem().string(), node_parent(*scene_.get().root)};
    		auto new_item = new QStandardItem{};
    		new_item->setData(new_node->name().data(), Qt::EditRole);
    		new_item->setData(QVariant::fromValue(any_ref{*new_node}), Qt::UserRole);
    		model_->appendRow(new_item);
    		emit new_node_created(*new_node);
    	}
    }

    auto hierarchy::import_3Dmodel_to_current_node() -> void {
    	nifo_expect(associated_arena_); // precondition

    	if (not tree_view_->selectionModel()->currentIndex().isValid()) return import_3Dmodel_to_current_scene();
    	auto current_item_index = tree_view_->selectionModel()->currentIndex();
    	auto current_item = model_->itemFromIndex(current_item_index);
    	auto& current_node = current_item->data(Qt::UserRole).value<any_ref>().as<nifo::node>();

    	scoped_make_current _{*associated_arena_};
    	for (const auto& presets = model_import_dialog_->get_selected_models_information(); const auto& preset : presets) {
    		emit node_importing_started(QString::fromStdString(preset.file.string()));
    		auto new_node = new node{associated_arena_->import_3Dmodel(preset), preset.file.stem().string(), node_parent(current_node)};
    		auto new_item = new QStandardItem{};
    		new_item->setData(new_node->name().data(), Qt::EditRole);
    		new_item->setData(QVariant::fromValue(any_ref{*new_node}), Qt::UserRole);
    		current_item->appendRow(new_item);
    		tree_view_->expand(current_item_index);
    		emit new_node_created(*new_node);
    	}
    }

    auto hierarchy::delete_current_node() -> void { // NOLINT: I don't need fucking `const`
    	nifo_expect(associated_arena_); // precondition

    	auto current_item_index = tree_view_->selectionModel()->currentIndex();
    	if (not current_item_index.isValid()) return;
    	auto current_item = model_->itemFromIndex(current_item_index);
    	auto& current_node = current_item->data(Qt::UserRole).value<any_ref>().as<nifo::node>();
    	associated_arena_->delete_object_(current_node.id());
    	current_node.remove_self();
    	model_->removeRow(current_item_index.row(), current_item_index.parent());
    	associated_arena_->repaint(); // NOLINT: essure `assosiated_arenas_` not null on this member function invoked
    }

    auto hierarchy::select(node& current) -> void {
    	nifo_expect(associated_arena_);

    	std::deque<QModelIndex> model_indice;
    	std::ranges::transform(std::views::iota(0, model_->rowCount()), std::back_inserter(model_indice), [this] (int i) {return model_->index(i, 0);});
	    QModelIndex index;
    	while (not model_indice.empty()) {
    		auto front = model_indice.front();
    		model_indice.pop_front();
    		if (model_->data(front, Qt::UserRole).value<any_ref>() == any_ref{current}) {
    			index = front;
    			break;
    		}
    		for (int i = 0; i < model_->rowCount(front); ++i) {
    			model_indice.push_back(model_->index(i, 0, front));
    		}
    	}
    	nifo_expect(index.isValid());
    	tree_view_->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    	tree_view_->setCurrentIndex(index);
    	tree_view_->scrollTo(index);
    	emit current_changed(current);
    }

    auto hierarchy::build_tree_model_(const node::child_node_array& nodes) -> QStandardItemModel* {
    	auto result = std::make_unique<QStandardItemModel>(this);
		for (auto& node : nodes) {
			auto item = new QStandardItem{};
			item->setData(QString(node->name().data()), Qt::EditRole);
			item->setData(QVariant::fromValue(any_ref{*node}), Qt::UserRole);
			result->appendRow(item);
			do_build_tree_model_(node->children(), item);
		}
    	return result.release();
    }

    auto hierarchy::do_build_tree_model_(const node::child_node_array& nodes, QStandardItem* parent) -> void { // NOLINT: the recursive invocation is wanted
    	for (auto& node : nodes) {
    		auto item = new QStandardItem{};
    		item->setData(QString(node->name().data()), Qt::EditRole);
    		item->setData(QVariant::fromValue(any_ref{*node}), Qt::UserRole);
    		parent->appendRow(item);
    		do_build_tree_model_(node->children(), item);
    	}
    }
}
