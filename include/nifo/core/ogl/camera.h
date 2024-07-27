#pragma once
#include <array>
#include <numbers>
#include "vertex_buffer.h"

namespace nifo {
	class camera {
	public:
		
		camera(glm::vec3 position, float aspect_ratio, float fovy,  float near, float far, glm::vec3 center) noexcept;

		[[nodiscard]]
		auto view_matrix() const noexcept ->glm::mat4;
		
		[[nodiscard]]
		auto projection_matrix() const noexcept ->glm::mat4;
		
		[[nodiscard]]
		auto view_projection_matrix() const noexcept ->glm::mat4;
		
		auto swap(camera& other) noexcept ->void;
		
		friend auto swap(camera& lhs, camera& rhs) noexcept ->void {
			return lhs.swap(rhs);
		}

		[[nodiscard]]
		auto aspect_ratio() const noexcept ->float;

		[[nodiscard]]
		auto fovy() const noexcept ->float;

		[[nodiscard]]
		auto near_distance() const noexcept ->float;

		[[nodiscard]]
		auto far_distance() const noexcept ->float;

		[[nodiscard]]
		auto front() const noexcept ->glm::vec3;

		[[nodiscard]]
		auto up() const noexcept ->glm::vec3;

		[[nodiscard]]
		auto right() const noexcept ->glm::vec3;

		[[nodiscard]]
		auto position() const noexcept ->glm::vec3;

		auto look_at(glm::vec3 center) noexcept ->void;

		auto orbit(glm::vec3 center, glm::vec2 delta) noexcept ->void;

		auto drift(glm::vec2 delta) noexcept ->void;

		auto zoom(float a) noexcept ->void;

		auto move_by(glm::vec3 distance) noexcept ->void;

		auto move_to(glm::vec3 destination) noexcept ->void;

		auto set_aspect_ratio(float aspect_ratio) noexcept ->void;

		auto set_near(float new_near) noexcept ->void;
		
		auto set_far(float new_far) noexcept ->void;
		
		auto set_fovy(float new_fovy) noexcept ->void;
	private:
		glm::vec3 position_;
		glm::vec3 front_{};
		glm::vec3 up_{};
		glm::vec3 right_{};
		float aspect_ratio_;
		float fovy_{};
		float near_{};
		float far_{};
		float zoomed_fovy_ = fovy_;
	};
}