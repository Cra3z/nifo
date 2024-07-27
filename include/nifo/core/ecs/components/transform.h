#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common.h"

namespace nifo {
	class node;

	enum class space : bool {
		local = false,
		world = true
	};
}

namespace nifo::components {

	struct transform;

	struct cached_transform_info {
		bool       valid_ = false;
		glm::mat4  model_matrix_{1.f};
		glm::vec3  global_position_{0.f};
	};

	struct transform {

		constexpr static glm::mat4 identity_matrix{1.f};

		[[nodiscard]]
		auto local_to_world() noexcept ->glm::mat4;

		[[nodiscard]]
		auto world_to_local() noexcept ->glm::mat4;

		[[nodiscard]]
		auto global_position() noexcept ->glm::vec3;

		auto translate(glm::vec3 translation, space relative_to = space::local) noexcept ->void;

		auto rotate(glm::vec3 euler_angles, space relative_to = space::local) noexcept ->void;

		auto scale(glm::vec3 scalings) noexcept ->void;

		auto refresh_cache() noexcept ->void;

		auto update_cached_info_if_invalid() noexcept ->void;

		cached_transform_info cached_;

		std::pair<std::reference_wrapper<scene>, entt::entity> owner;

		glm::vec3 local_position{0.f};
		glm::vec3 local_rotation{0.f};
		glm::vec3 local_scale{1.f};

		glm::vec3 front{0.f, 0.f, 1.f};
		glm::vec3 right{1.f, 0.f, 0.f};
		glm::vec3    up{0.f, 1.f, 0.f};
	};

	static_assert(std::is_aggregate_v<transform>);
}