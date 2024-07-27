#include <QApplication>
#include <QMessageBox>
#include <nifo/ui/main_window.h>

auto main(int argc, char* argv[]) -> int {
	QApplication app{argc, argv};
	QFile style_sheet{":/styles/default.qss"};
	style_sheet.open(QFile::ReadOnly);
	app.setStyleSheet(style_sheet.readAll());
	int exit_code = -1;
	try {
		nifo::ui::main_window main_window{};
		main_window.setWindowTitle("nifo");
		main_window.setMinimumSize(1200, 840);
		main_window.showMaximized();
		exit_code = app.exec();
	}
	catch (const nifo::runtime_error& e) {
		QMessageBox::critical(nullptr, "运行时错误", QString::fromStdString(e.what()));
		spdlog::error("{}", e.what());
	}
	return exit_code;
}
