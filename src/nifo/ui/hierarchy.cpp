#include <deque>
#include <QHeaderView>
#include <QScreen>
#include <nifo/core/utils/any_ref.h>
#include <nifo/ui/hierarchy.h>
#include <nifo/ui/arena.h>
#include <nifo/ui/inspector.h>
#include <nifo/ui/for_qss.h>
#include <nifo/core/ogl/glsl_structs.h>

namespace nifo::ui {

    hierarchy::hierarchy(scene& scene, QWidget* parent) :
		QWidget(parent),
		scene_(scene),
		menu_(new QMenu{this}),
		model_(build_tree_model_(scene.root->children())),
		tree_view_(new QTreeView{this}),
		model_import_dialog_(new import_3dmodel_dialog(this, QStringLiteral("导入3D模型"), QStringLiteral("3D模型文件(*.obj *.fbx)")))
	{
    	tree_view_->setModel(model_);
    	tree_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    	model_->setHorizontalHeaderLabels(QStringList{QString::fromStdString(std::format("{}", scene.name))});
		auto sub_menu_1 = menu_->addMenu("在场景中新建对象");
    	sub_menu_1->addAction(QIcon{":/images/model-dark.svg"}, "3D模型", this, &hierarchy::import_3Dmodel_to_current_scene);
    	sub_menu_1->addAction(QIcon{":/images/parallel-light.png"}, "平行光", [this]{this->add_light_to_current_scene(0);});
    	sub_menu_1->addAction(QIcon{":/images/point-light.png"}, "点光源", [this]{this->add_light_to_current_scene(1);});
    	sub_menu_1->addAction(QIcon{":/images/spot-light.png"}, "聚光灯", [this]{this->add_light_to_current_scene(2);});

    	auto sub_menu_2 = menu_->addMenu("添加子对象");
    	sub_menu_2->addAction(QIcon{":/images/model-dark.svg"}, "3D模型", this, &hierarchy::import_3Dmodel_to_current_object);
    	sub_menu_2->addAction(QIcon{":/images/parallel-light.png"}, "平行光", [this]{this->add_light_to_current_object(0);});
    	sub_menu_2->addAction(QIcon{":/images/point-light.png"}, "点光源", [this]{this->add_light_to_current_object(1);});
    	sub_menu_2->addAction(QIcon{":/images/spot-light.png"}, "聚光灯", [this]{this->add_light_to_current_object(2);});

    	menu_->addSeparator();
    	menu_->addAction(QIcon{":/images/delete-dark.svg"}, "删除对象", this, &hierarchy::delete_current_node);
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
    		emit model_importing_started(QString::fromStdString(preset.file.string()));
    		auto new_node = new node{associated_arena_->import_3Dmodel(preset), preset.file.stem().string(), node_parent(*scene_.get().root)};
    		auto new_item = new QStandardItem{};
    		new_item->setData(new_node->name().data(), Qt::EditRole);
    		new_item->setData(QVariant::fromValue(any_ref{*new_node}), Qt::UserRole);
    		model_->appendRow(new_item);
    		emit model_importing_done(*new_node);
    	}
    }

    auto hierarchy::import_3Dmodel_to_current_object() -> void {
    	nifo_expect(associated_arena_); // precondition

    	if (not tree_view_->selectionModel()->currentIndex().isValid()) return import_3Dmodel_to_current_scene();

    	auto current_item_index = tree_view_->selectionModel()->currentIndex();
    	auto current_item = model_->itemFromIndex(current_item_index);
    	auto& current_node = current_item->data(Qt::UserRole).value<any_ref>().as<nifo::node>();

    	scoped_make_current _{*associated_arena_};
    	for (const auto& presets = model_import_dialog_->get_selected_models_information(); const auto& preset : presets) {
    		emit model_importing_started(QString::fromStdString(preset.file.string()));
    		auto new_node = new node{associated_arena_->import_3Dmodel(preset), preset.file.stem().string(), node_parent(current_node)};
    		auto new_item = new QStandardItem{};
    		new_item->setData(new_node->name().data(), Qt::EditRole);
    		new_item->setData(QVariant::fromValue(any_ref{*new_node}), Qt::UserRole);
    		current_item->appendRow(new_item);
    		tree_view_->expand(current_item_index);
    		emit model_importing_done(*new_node);
    	}
    }

    auto hierarchy::add_light_to_current_scene(int category) -> void {
    	nifo_expect(associated_arena_); // precondition

		auto new_node = add_light_impl(category, *scene_.get().root);
    	auto new_item = new QStandardItem{};
    	new_item->setData(new_node->name().data(), Qt::EditRole);
    	new_item->setData(QVariant::fromValue(any_ref{*new_node}), Qt::UserRole);
    	model_->appendRow(new_item);
    	associated_arena_->update();
    }

    auto hierarchy::add_light_to_current_object(int category) -> void {
    	nifo_expect(associated_arena_); // precondition

    	if (not tree_view_->selectionModel()->currentIndex().isValid()) return add_light_to_current_scene(category);

    	auto current_item_index = tree_view_->selectionModel()->currentIndex();
    	auto current_item = model_->itemFromIndex(current_item_index);
    	auto& current_object = current_item->data(Qt::UserRole).value<any_ref>().as<nifo::node>();

    	auto new_node = add_light_impl(category, current_object);
    	auto new_item = new QStandardItem{};
    	new_item->setData(new_node->name().data(), Qt::EditRole);
    	new_item->setData(QVariant::fromValue(any_ref{*new_node}), Qt::UserRole);
    	current_item->appendRow(new_item);
    	tree_view_->expand(current_item_index);
    	associated_arena_->update();
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

    auto hierarchy::refrush_current() -> void {
    	auto current_item_index = tree_view_->selectionModel()->currentIndex();
    	auto current_item = model_->itemFromIndex(current_item_index);
    	auto& current_object = current_item->data(Qt::UserRole).value<any_ref>().as<nifo::node>();
    	current_item->setData(QString::fromStdString(current_object.name().data()), Qt::DisplayRole);
    }

    auto hierarchy::build_tree_model_(const node::child_node_array& nodes) -> QStandardItemModel* {
    	auto result = std::make_unique<QStandardItemModel>(this);
		for (auto& node : nodes) {
			auto item = new QStandardItem{};
			item->setData(QString(node->name().data()), Qt::DisplayRole);
			item->setData(QVariant::fromValue(any_ref{*node}), Qt::UserRole);
			result->appendRow(item);
			do_build_tree_model_(node->children(), item);
		}
    	return result.release();
    }

    auto hierarchy::do_build_tree_model_(const node::child_node_array& nodes, QStandardItem* parent) -> void { // NOLINT: the recursive invocation is wanted
    	for (auto& node : nodes) {
    		auto item = new QStandardItem{};
    		item->setData(QString(node->name().data()), Qt::DisplayRole);
    		item->setData(QVariant::fromValue(any_ref{*node}), Qt::UserRole);
    		parent->appendRow(item);
    		do_build_tree_model_(node->children(), item);
    	}
    }

    auto hierarchy::add_light_impl(int category, node& parent) -> node* {
    	nifo_expect(associated_arena_); // precondition

    	static const std::vector<pair<std::string_view, std::function<void(node*)>>> light_categories{
			{"Parallel Light", &node::add_component<components::parallel_light>},
			{"Point Light", &node::add_component<components::point_light>},
			{"Spot Light", &node::add_component<components::spot_light>},
    	};

    	auto [name, add_light_comp] = light_categories.at(category);
    	auto new_object = new node{name, node_parent(parent)};
    	add_light_comp(new_object);
    	return new_object;
    }
}
