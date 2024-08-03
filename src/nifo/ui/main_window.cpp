#include <QApplication>
#include <QTimer>
#include <QMenuBar>
#include <QActionGroup>
#include <QToolBar>
#include <nifo/ui/main_window.h>

namespace nifo::ui {
	main_window::main_window() {
		using namespace std::chrono_literals;
		scene_manager_.create_new_scene("New Scene");
		splitter_ = new QSplitter{this};
		hierarchy_ = new hierarchy{scene_manager_.current_scene(), splitter_};
		arena_ = new arena{*hierarchy_, splitter_};
		inspector_ = new inspector{*hierarchy_, splitter_};
		tool_bar_ = new QToolBar{this};
		status_bar_ = new QStatusBar{this};
		progress_bar_ = new QProgressBar{status_bar_};

		splitter_->addWidget(hierarchy_);
		splitter_->addWidget(arena_);
		splitter_->addWidget(inspector_);
		splitter_->setStretchFactor(0, 1);
		splitter_->setStretchFactor(1, 4);
		splitter_->setStretchFactor(2, 1);

		menu_bar_ = new QMenuBar{this};

		status_bar_->addPermanentWidget(progress_bar_);

		progress_bar_->setOrientation(Qt::Horizontal);
		progress_bar_->setRange(0, 100);
		progress_bar_->setValue(0);
		progress_bar_->hide();

		timer_ = new QTimer{this};
		timer_->setInterval(1s);

		tool_bar_->setFloatable(false);
		tool_bar_->setMovable(false);
		auto left_button_behavior_group = new QActionGroup{this};
		left_button_behavior_group->setExclusive(true);
		std::vector<std::tuple<std::string_view, std::string_view, arena::left_button_mouse_behavior>> action_group1_arguments {
			{":images/select.png",    "选择", arena::select},
			{":images/translate.png", "移动", arena::translate},
			{":images/rotate.png",    "旋转", arena::rotate},
			{":images/scale.png",     "缩放", arena::scale}
		};
		for (bool first = true; const auto& [icon, text, behavior] : action_group1_arguments) {
			auto action = left_button_behavior_group->addAction(
				tool_bar_->addAction(QIcon{icon.data()}, text.data(), [=, this] (bool checked) {
					if (checked) arena_->set_left_button_behavior(behavior);
				})
			);
			action->setCheckable(true);
			if (first) {
				action->setChecked(true);
				first = false;
			}
		}
		tool_bar_->addSeparator(); // --------------------------
		auto view_mode_group = new QActionGroup{this};
		view_mode_group->setExclusive(true);
		std::vector<std::tuple<std::string_view, std::string_view, arena::polygon_mode>> action_group2_arguments {
			{":images/fill.png",  "填充", arena::fill},
			{":images/line.png",  "线框", arena::line},
			{":images/point.png", "顶点", arena::point}
		};
		for (bool first = true; const auto& [icon, text, mode] : action_group2_arguments) {
			auto action = view_mode_group->addAction(
				tool_bar_->addAction(QIcon{icon.data()}, text.data(), [=, this] (bool checked) {
					if (checked) arena_->set_view_mode(mode);
				})
			);
			action->setCheckable(true);
			if (first) {
				action->setChecked(true);
				first = false;
			}
		}
		tool_bar_->addSeparator(); // --------------------------

		addToolBar(Qt::TopToolBarArea, tool_bar_);
		setCentralWidget(splitter_);
		setStatusBar(status_bar_);

		connect(inspector_, &inspector::new_value_inputed_in_some_panel, [this] (panel_base::next_to_do next_to_do) {
			switch (next_to_do) {
			case panel_base::next_to_do::refrush_view:
				hierarchy_->refrush_current();
				break;
			case panel_base::next_to_do::repaint_scene:
				arena_->repaint();
				break;
			default:
				break;
			}
		});
		connect(hierarchy_, &hierarchy::current_changed, inspector_, &inspector::bind);
		connect(hierarchy_, &hierarchy::no_selected, inspector_, &inspector::hide_panels);
		connect(timer_, &QTimer::timeout, [this] {
			if (timer_->isActive()) {
				progress_bar_->setValue(std::min(progress_bar_->value() + 33, 99));
			}
		});
		connect(hierarchy_, &hierarchy::model_importing_started, [this] (const QString& message) {
			status_bar_->showMessage("导入模型" + message + "中...");
			progress_bar_->show();
			progress_bar_->setValue(0);
			timer_->start();
		});
		connect(hierarchy_, &hierarchy::model_importing_done, [this] {
			timer_->stop();
			progress_bar_->setValue(100);
			progress_bar_->hide();
			status_bar_->clearMessage();
			arena_->repaint();
		});
		auto file_menu = menu_bar_->addMenu("文件");
		file_menu->addMenu(QIcon{":/images/new.svg"}, "新建项目");
		file_menu->addAction(QIcon{":/images/folder-dark.svg"}, "打开项目");
		file_menu->addAction("关闭项目");
		file_menu->addSeparator();
		file_menu->addAction(QIcon{":/images/settings-dark.svg"}, "设置");
		file_menu->addSeparator();
		file_menu->addAction(QIcon{":/images/save-dark.svg"}, "保存项目");
		file_menu->addMenu("另存为");
		file_menu->addSeparator();
		file_menu->addAction(QIcon{":/images/model-dark.svg"}, "导入3D模型", hierarchy_, &hierarchy::import_3Dmodel_to_current_scene);
		file_menu->addMenu("将项目导出为");
		file_menu->addSeparator();
		file_menu->addAction(QIcon{":/images/exit-dark.svg"}, "退出", QApplication::quit);

		auto edit_menu = menu_bar_->addMenu("编辑");
		edit_menu->addAction("撤销");
		edit_menu->addAction("重做");
		edit_menu->addSeparator();
		edit_menu->addAction("复制");
		edit_menu->addAction("粘贴");
		edit_menu->addAction("删除");

		auto tool_menu = menu_bar_->addMenu("工具");
		tool_menu->addAction(QIcon{":/images/vscode.png"}, "打开VS Code");
		tool_menu->addAction(QIcon{":/images/python-console-dark.svg"}, "打开Python控制台");

		menu_bar_->addAction("关于");
		setMenuBar(menu_bar_);
	}

	main_window::~main_window() {
		{
			scoped_make_current _{*arena_};
			scene_manager_.clear();
			delete hierarchy_;
			delete inspector_;
		}
		delete arena_;
	}
}
