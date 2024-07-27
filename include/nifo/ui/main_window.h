#pragma once
#include <QMainWindow>
#include <QStatusBar>
#include <QSplitter>
#include <QProgressBar>
#include <nifo/ui/hierarchy.h>
#include <nifo/ui/arena.h>
#include <nifo/ui/inspector.h>

namespace nifo::ui {
	class main_window : public QMainWindow {
	public:

		using QMainWindow::QMainWindow;

		main_window();

		~main_window() override;

	private:
		QSplitter* splitter_;
		arena* arena_;
		hierarchy* hierarchy_;
		inspector* inspector_;
		QMenuBar* menu_bar_;
		QToolBar* tool_bar_;
		QStatusBar* status_bar_;
		QProgressBar* progress_bar_;
		QTimer* timer_;
		scene_manager scene_manager_;
	};
}