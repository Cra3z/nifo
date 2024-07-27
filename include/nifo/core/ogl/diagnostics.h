#pragma once
#include <cassert>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/ranges.h>

#define nifo_expect(expr) assert(expr);

namespace nifo {
	struct runtime_error : std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	struct bad_resource_acquisition : runtime_error {
		using runtime_error::runtime_error;
	};

	struct opengl_api_error : runtime_error {
		using runtime_error::runtime_error;
	};

	struct assertion_failed_error : runtime_error {
		using runtime_error::runtime_error;
	};
}