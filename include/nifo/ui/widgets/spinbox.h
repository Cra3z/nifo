#pragma once
#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMouseEvent>

namespace nifo::ui {

	class basic_spinbox : public QWidget {
		Q_OBJECT
	public:
		explicit basic_spinbox(QWidget* parent);

	protected:
		auto enterEvent(QEnterEvent* event) ->void override;

		auto leaveEvent(QEvent* event) ->void override;

		auto eventFilter(QObject* watched, QEvent* event) ->bool override;

		auto set_new_validator(QValidator* validator) ->void ;

		auto update_text() ->void;
	private:

		virtual auto value_to_qstring() ->QString = 0;

		virtual auto set_value_from_qstring(const QString&) ->void = 0;

		virtual auto do_increase() ->void = 0;

		virtual auto do_decrease() ->void = 0;

		virtual auto can_inc() noexcept ->bool = 0;

		virtual auto can_dec() noexcept ->bool = 0;

	public slots:
		auto increase() ->void;

		auto decrease() ->void;

	private slots:
		auto finish_editing() ->void ;

	private:
		QLabel* label;
		QLineEdit* line_edit;
		QPushButton* increase_button;
		QPushButton* decrease_button;
		bool editing;
		bool dragging;
		int current_x = 0;
	};

	class integer_spinbox final : public basic_spinbox {
		Q_OBJECT
	public:

		explicit integer_spinbox(QWidget* parent = nullptr);

		[[nodiscard]]
		auto value() const noexcept ->int ;

		[[nodiscard]]
		auto maximum() const noexcept ->int ;

		[[nodiscard]]
		auto minimum() const noexcept ->int ;

		[[nodiscard]]
		auto single_step() const noexcept ->int ;

		auto set_value(int value) ->void;

		auto set_maximum(int max_value) ->void ;

		auto set_minimum(int min_value) ->void ;

		auto set_single_step(int step) ->void ;

		auto set_range(int min_value, int max_value, int step = 1) ->void ;

	private:
		auto value_to_qstring() -> QString override;

		auto set_value_from_qstring(const QString& str) -> void override;

		auto do_decrease() -> void override;

		auto do_increase() -> void override;

		auto can_dec() noexcept -> bool override;

		auto can_inc() noexcept -> bool override;

	signals:
		auto value_changed(int value) ->void;

	private:
		int value_ = 0;
		int min_ = std::numeric_limits<int>::lowest(), max_ = std::numeric_limits<int>::max();
		int step_ = 1;
	};


	class floating_point_spinbox final : public basic_spinbox {
		Q_OBJECT
	public:

		explicit floating_point_spinbox(QWidget* parent = nullptr, int precision = 2);

		[[nodiscard]]
		auto value() const noexcept ->double ;

		[[nodiscard]]
		auto maximum() const noexcept ->double ;

		[[nodiscard]]
		auto minimum() const noexcept ->double ;

		[[nodiscard]]
		auto single_step() const noexcept ->double ;

		auto set_value(double value) ->void;

		auto set_maximum(double max_value) ->void ;

		auto set_minimum(double min_value) ->void ;

		auto set_single_step(double step) ->void ;

		auto set_range(double min_value, double max_value, double step = 1.) ->void ;

	private:
		auto value_to_qstring() -> QString override;

		auto set_value_from_qstring(const QString& str) -> void override;

		auto do_decrease() -> void override;

		auto do_increase() -> void override;

		auto can_dec() noexcept -> bool override;

		auto can_inc() noexcept -> bool override;

	signals:
		auto value_changed(double value) ->void;

	private:
		double value_ = 0.0;
		double min_ = std::numeric_limits<double>::lowest(), max_ = std::numeric_limits<double>::max();
		double step_ = 0.1;
		int precision_;
	};
}