#pragma once
#include <QWidget>

namespace nifo::ui {

	inline auto set_usage(QObject* qobj, const char* usage) ->void {
		qobj->setProperty("nifo-usage", usage);
	}

	auto set_qss_property(QWidget* widget, const char* name, QVariant&& value) ->void;

}
