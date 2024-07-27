#pragma once
#include <concepts>
#include <entt/entt.hpp>
#include <vector>
#include "../../utils/core.h"

namespace nifo {

	class scene;

	template<typename T>
	concept component = std::copyable<T>;

	template<typename T>
	concept owner_aware_component = component<T> && requires(T t) {
		t.owner;
		requires std::same_as<std::remove_cvref_t<decltype(t.owner)>, std::pair<std::reference_wrapper<scene>, entt::entity>>;
	};
}