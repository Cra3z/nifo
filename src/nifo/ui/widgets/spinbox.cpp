#include <QHBoxLayout>
#include <QSpinBox>
#include <nifo/ui/widgets/spinbox.h>
#include <nifo/ui/for_qss.h>

namespace nifo::ui {
	basic_spinbox::basic_spinbox(QWidget* parent): QWidget(parent), editing(false), dragging(false) {
		setMouseTracking(true);
		label = new QLabel(this);
		label->setText("0");
		label->setAlignment(Qt::AlignCenter);
		label->installEventFilter(this);
		line_edit = new QLineEdit(this);
		line_edit->hide();
		line_edit->setAlignment(Qt::AlignVCenter);
		line_edit->installEventFilter(this);
		decrease_button = new QPushButton("-", this);
		set_usage(decrease_button, "decrease-indicator");
		decrease_button->hide();
		increase_button = new QPushButton("+", this);
		set_usage(increase_button, "increase-indicator");
		increase_button->hide();
		auto layout = new QHBoxLayout(this);
		layout->addWidget(decrease_button);
		layout->addWidget(label);
		layout->addWidget(line_edit);
		layout->addWidget(increase_button);
		layout->setSpacing(0);
		setLayout(layout);
		decrease_button->setMinimumWidth(10);
		increase_button->setMinimumWidth(10);
		label->setMinimumWidth(100);
		line_edit->setMinimumWidth(100);
		setMinimumWidth(120);
		setMaximumHeight(40);
		connect(increase_button, &QPushButton::clicked, this, &basic_spinbox::increase);
		connect(decrease_button, &QPushButton::clicked, this, &basic_spinbox::decrease);
		connect(line_edit, &QLineEdit::returnPressed, this, &basic_spinbox::finish_editing);
	}

	auto basic_spinbox::enterEvent(QEnterEvent* event) ->void {
		if (not editing) {
			increase_button->show();
			decrease_button->show();
		}
		QWidget::enterEvent(event);
	}

	auto basic_spinbox::leaveEvent(QEvent* event) ->void {
		if (not editing) {
			increase_button->hide();
			decrease_button->hide();
		}
		QWidget::leaveEvent(event);
	}

	auto basic_spinbox::eventFilter(QObject* watched, QEvent* event) ->bool {
		if (watched == label) {
			using enum QEvent::Type;
			switch (event->type()) {
			case MouseMove: {
				dragging = true;
				auto previous_x = std::exchange(current_x, static_cast<QMouseEvent*>(event)->pos().x());
				current_x > previous_x ? increase() : decrease();
				break;
			}
			case MouseButtonRelease:
				if (not dragging) {
					decrease_button->hide();
					label->hide();
					increase_button->hide();
					line_edit->setText(value_to_qstring());
					line_edit->show();
					line_edit->setFocus();
					editing = true;
				}
				dragging = false;
				break;
			case Enter:
				setCursor(Qt::SizeHorCursor);
				break;
			case Hide: [[fallthrough]];
			case Leave:
				dragging = false;
				unsetCursor();
				break;
			default:
				; // ignored
			}
		}
		else if (watched == line_edit) {
			using enum QEvent::Type;
			switch (event->type()) {
			case FocusOut:
				finish_editing();
			default:
				; // ignored
			}
		}
		return false;
	}

	auto basic_spinbox::set_new_validator(QValidator* validator) -> void {
		line_edit->setValidator(validator);
	}

	auto basic_spinbox::update_text() -> void {
		label->setText(value_to_qstring());
		line_edit->setText(label->text());
	}

	auto basic_spinbox::increase() -> void {
		if (can_inc()) {
			do_increase();
			update_text();
		}
	}

	auto basic_spinbox::decrease() -> void {
		if (can_dec()) {
			do_decrease();
			update_text();
		}
	}

	auto basic_spinbox::finish_editing() -> void {
		if (editing) {
			set_value_from_qstring(line_edit->text());
			label->show();
			line_edit->hide();
			editing = false;
		}
	}

	integer_spinbox::integer_spinbox(QWidget* parent): basic_spinbox(parent) {}

	auto integer_spinbox::value() const noexcept -> int {
		return value_;
	}

	auto integer_spinbox::maximum() const noexcept -> int {
		return max_;
	}

	auto integer_spinbox::minimum() const noexcept -> int {
		return min_;
	}

	auto integer_spinbox::single_step() const noexcept -> int {
		return step_;
	}

	auto integer_spinbox::set_value(int value) -> void {
		value_ = value;
		update_text();
		emit value_changed(value_);
	}

	auto integer_spinbox::set_maximum(int max_value) -> void {
		max_ = max_value;
		assert(max_ >= min_);
		set_new_validator(new QIntValidator{min_, max_});
	}

	auto integer_spinbox::set_minimum(int min_value) -> void {
		min_ = min_value;
		assert(max_ >= min_);
		set_new_validator(new QIntValidator{min_, max_});
	}

	auto integer_spinbox::set_single_step(int step) -> void {
		step_ = step;
		assert(step_ > 0);
	}

	auto integer_spinbox::set_range(int min_value, int max_value, int step) -> void {
		max_ = max_value;
		assert(max_ >= min_);
		min_ = min_value;
		assert(max_ >= min_);
		step_ = step;
		assert(step_ > 0);
		set_new_validator(new QIntValidator{min_, max_});
	}

	auto integer_spinbox::value_to_qstring() -> QString {
		return QString::number(value_);
	}

	auto integer_spinbox::set_value_from_qstring(const QString& str) -> void {
		set_value(str.toInt());
	}

	auto integer_spinbox::do_decrease() -> void {
		value_ -= step_;
		emit value_changed(value_);
	}

	auto integer_spinbox::do_increase() -> void {
		value_ += step_;
		emit value_changed(value_);
	}

	auto integer_spinbox::can_dec() noexcept -> bool {
		return value_ > min_ and value_ - step_ >= min_;
	}

	auto integer_spinbox::can_inc() noexcept -> bool {
		return value_ < max_ and value_ + step_ <= max_;
	}

	floating_point_spinbox::floating_point_spinbox(QWidget* parent, int precision): basic_spinbox(parent), precision_(precision) {}

	auto floating_point_spinbox::value() const noexcept -> double {
		return value_;
	}

	auto floating_point_spinbox::maximum() const noexcept -> double {
		return max_;
	}

	auto floating_point_spinbox::minimum() const noexcept -> double {
		return min_;
	}

	auto floating_point_spinbox::single_step() const noexcept -> double {
		return step_;
	}

	auto floating_point_spinbox::set_value(double value) -> void {
		value_ = value;
		update_text();
		emit value_changed(value_);
	}

	auto floating_point_spinbox::set_maximum(double max_value) -> void {
		max_ = max_value;
		assert(max_ >= min_);
		set_new_validator(new QDoubleValidator{min_, max_, precision_});
	}

	auto floating_point_spinbox::set_minimum(double min_value) -> void {
		min_ = min_value;
		assert(max_ >= min_);
		set_new_validator(new QDoubleValidator{min_, max_, precision_});
	}

	auto floating_point_spinbox::set_single_step(double step) -> void {
		step_ = step;
		assert(step_ > 0.);
	}

	auto floating_point_spinbox::set_range(double min_value, double max_value, double step) -> void {
		max_ = max_value;
		assert(max_ >= min_);
		min_ = min_value;
		assert(max_ >= min_);
		step_ = step;
		assert(step_ > 0.);
		set_new_validator(new QDoubleValidator{min_, max_, precision_});
	}

	auto floating_point_spinbox::value_to_qstring() -> QString {
		return QString::number(value_, 'f', precision_);
	}

	auto floating_point_spinbox::set_value_from_qstring(const QString& str) -> void {
		set_value(str.toDouble());
	}

	auto floating_point_spinbox::do_decrease() -> void {
		value_ -= step_;
		emit value_changed(value_);
	}

	auto floating_point_spinbox::do_increase() -> void {
		value_ += step_;
		emit value_changed(value_);
	}

	auto floating_point_spinbox::can_dec() noexcept -> bool {
		return value_ > min_ and value_ - step_ >= min_;
	}

	auto floating_point_spinbox::can_inc() noexcept -> bool {
		return value_ < max_ and value_ + step_ <= max_;
	}
}