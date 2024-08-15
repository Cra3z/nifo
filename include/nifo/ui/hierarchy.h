#pragma once
#include <memory>
#include <QMenu>
#include <QFileDialog>
#include <QTreeView>
#include <QStandardItemModel>
#include "../core/scene.h"
#include "widgets/import_3dmodel_dialog.h"

namespace nifo::ui {
    
    class hierarchy : public QWidget {
        Q_OBJECT

    	friend class arena;

    	friend class inspector;

    public:
    
        explicit hierarchy(scene& scene, QWidget* parent = nullptr);
        
        ~hierarchy() override = default;

    	auto resizeEvent(QResizeEvent *event) ->void override;

	public slots:
    	auto import_3Dmodel_to_current_scene() ->void;

    	auto import_3Dmodel_to_current_object() ->void;

		auto add_light_to_current_scene(int category) ->void;

    	auto add_light_to_current_object(int category) ->void;

    	auto delete_current_node() ->void;

    	auto select(node& current) ->void;

    	auto refrush_current() ->void;
	signals:

    	auto current_changed(node&) ->void;

    	auto no_selected() ->void;

    	auto model_importing_started(QString filename) ->void;

    	auto model_importing_done(node& new_node) ->void;
	private:

		auto build_tree_model_(const node::child_node_array& nodes) -> QStandardItemModel*;

		auto do_build_tree_model_(const node::child_node_array& nodes, QStandardItem* parent) ->void;

		auto add_light_impl(int category, node& parent) -> node*;

    private:
    	std::reference_wrapper<scene> scene_;
		QMenu* menu_;
		QStandardItemModel* model_;
		QTreeView* tree_view_;
    	import_3dmodel_dialog* model_import_dialog_;
    	inspector* associated_inspector_ = nullptr;
    	arena* associated_arena_ = nullptr;
    };

}
