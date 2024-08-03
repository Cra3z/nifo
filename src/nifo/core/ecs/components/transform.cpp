#include <nifo/core/ecs/components/transform.h>
#include <nifo/core/scene.h>

namespace nifo::components {

	auto transform::local_to_world() noexcept -> glm::mat4 {
		update_cached_info_if_invalid();
		return cached_.model_matrix_;
	}

	auto transform::global_position() noexcept -> glm::vec3 {
		update_cached_info_if_invalid();
		return cached_.global_position_;
	}

	auto transform::translate(glm::vec3 translation, space relative_to) noexcept -> void {
		if (relative_to == space::local) local_position += translation;
		else local_position += glm::vec3(world_to_local() * glm::vec4{translation, 1.f});
	}

	auto transform::rotate(glm::vec3 euler_angles, space relative_to) noexcept -> void {
		if (relative_to == space::local) local_rotation += euler_angles;
		else local_rotation += glm::vec3(world_to_local() * glm::vec4{euler_angles, 1.f});
	}

	auto transform::scale(glm::vec3 scaling) noexcept -> void {
		local_scale *= scaling;
	}

	auto transform::refresh_cache() noexcept -> void {
		cached_.valid_ = false;
	}

	auto transform::update_cached_info_if_invalid() noexcept -> void {
		if (not cached_.valid_) {
			auto S = glm::scale(glm::mat4{1.f}, local_scale);
			auto R = glm::rotate(glm::mat4{1.f}, glm::radians(local_rotation.z), glm::vec3{0.f, 0.f, 1.f});
			R = glm::rotate(glm::mat4{1.f}, glm::radians(local_rotation.x), glm::vec3{1.f, 0.f, 0.f}) * R;
			R = glm::rotate(glm::mat4{1.f}, glm::radians(local_rotation.y), glm::vec3{0.f, 1.f, 0.f}) * R;
			auto T = glm::translate(glm::mat4{1.f}, local_position);
			auto parent = owner.first.get().entities.at(owner.second).get().parent();
			cached_.rotation_matrix_ = R;
			cached_.model_matrix_ = T * R * S;
			cached_.global_position_ = local_position;
			if (parent and not parent->is_root()) {
				auto& parent_transform = parent->transform();
				auto parent_local_to_world = parent_transform.local_to_world();
				cached_.rotation_matrix_ = parent_transform.cached_.rotation_matrix_ * cached_.rotation_matrix_;
				cached_.model_matrix_ = parent_local_to_world * cached_.model_matrix_;
				cached_.global_position_ = parent_local_to_world * glm::vec4{cached_.global_position_, 1.f};
			}
			cached_.valid_ = true;
		}
	}
}
