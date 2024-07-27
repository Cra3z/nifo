#include <QApplication>
#include <QStyle>
#include <nifo/ui/for_qss.h>

namespace nifo::ui {

	auto set_qss_property(QWidget* widget, const char* name, QVariant&& value) -> void {
		widget->setProperty(name, std::move(value));
		qApp->style()->unpolish(widget);
		qApp->style()->polish(widget);
	}
}
