#include <nifo/core/utils/loop.h>
#include <spdlog/spdlog.h>

#ifdef NIFO_GUI_USE_QT
#include <QCoreApplication>
#include <QEventLoop>

namespace nifo {

	class loop_iteration_ final : public QObject {
	public:
		loop_iteration_(loop::body_type body, loop_controller controller) noexcept : body_(std::move(body)), controller_(controller) {}

		auto next() ->void {
			body_(controller_);
			if (not controller_.stop_requested()) {
				QCoreApplication::postEvent(this, new QEvent{QEvent::User});
			}
		}

	private:

		auto event(QEvent* e) ->bool override {
			if (e->type() == QEvent::User) {
				next();
				return true;
			}
			return QObject::event(e);
		}

	private:
		loop::body_type body_;
		loop_controller controller_;
	};

	struct loop::impl {
		impl(body_type body, const loop_controller& controller) noexcept : iteration{std::move(body), controller} {}

		auto start_iteration() ->void {
			iteration.next();
			qevent_loop.exec();
		}

		QEventLoop qevent_loop;
		loop_iteration_ iteration;
		bool stopped = false;
	};

	auto loop::request_stop() const -> void {
		pimpl->stopped = true;
		if (pimpl->qevent_loop.isRunning()) pimpl->qevent_loop.quit();
	}

	auto loop::yield() const -> void {
		pimpl->qevent_loop.processEvents(QEventLoop::ExcludeUserInputEvents);
	}

	auto loop::stop_requested() const noexcept -> bool {
		return pimpl->stopped;
	}

}

#endif

namespace nifo {

	loop::loop(body_type body) : pimpl(std::make_unique<impl>(std::move(body), loop_controller{*this})) {
		pimpl->start_iteration();
	}

	loop::~loop() = default;

	loop_controller::loop_controller(loop& loop_) : associated_loop_(loop_) {}

}
