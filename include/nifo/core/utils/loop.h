#pragma once
#include <memory>
#include "core.h"

namespace nifo {

	struct context_setter {
		virtual auto make_current() ->void = 0;

		virtual auto done_current() ->void = 0;

		virtual ~context_setter() = default;
	};

	class loop_controller;

	class loop {
		struct impl;
	public:
		using body_type = move_only_function<void(const loop_controller&)>;

	public:
		explicit loop(body_type body);

		auto request_stop() const ->void;

		auto yield() const ->void;

		[[nodiscard]]
		auto stop_requested() const noexcept ->bool;

		~loop();

	private:
		std::unique_ptr<impl> pimpl;
	};

	class loop_controller {
	public:
		explicit loop_controller(loop& loop_);

		auto request_stop() const ->void {
			associated_loop_.get().request_stop();
		}

		auto yield() const ->void {
			associated_loop_.get().yield();
		}

		[[nodiscard]]
		auto stop_requested() const noexcept ->bool {
			return associated_loop_.get().stop_requested();
		}
	private:
		std::reference_wrapper<loop> associated_loop_;
	};
}