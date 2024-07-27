#pragma once
#include <type_traits>
#include <concepts>

template<typename T> requires std::invocable<const T&>
struct lazy_evaluated : T {
	using value_type = std::invoke_result_t<const T&>;
	operator value_type() const noexcept(std::is_nothrow_invocable_v<const T&>) {
		return (*this)();
	}
};