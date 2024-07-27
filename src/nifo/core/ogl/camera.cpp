#include <nifo/core/ogl/camera.h>

namespace nifo {
	camera::camera(glm::vec3 position, float aspect_ratio, float fovy,  float near, float far, glm::vec3 center) noexcept :
		position_(position), aspect_ratio_(aspect_ratio), fovy_(fovy), near_(near), far_(far) {
		look_at(center);
	}
	
	auto camera::view_matrix() const noexcept -> glm::mat4 {
		return glm::lookAt(position_, position_ + front_, up_);
	}
	
	auto camera::projection_matrix() const noexcept -> glm::mat4 {
		return glm::perspective(zoomed_fovy_, aspect_ratio_, near_, far_);
	}
	
	auto camera::view_projection_matrix() const noexcept -> glm::mat4 {
		return projection_matrix() * view_matrix();
	}

	auto camera::swap(camera& other) noexcept ->void {
		std::ranges::swap(position_, other.position_);
		std::ranges::swap(front_, other.front_);
		std::ranges::swap(up_, other.up_);
		std::ranges::swap(right_, other.right_);
	}
	
	auto camera::look_at(glm::vec3 center) noexcept ->void {
		front_ = glm::normalize(center - position_);
		right_ = center[0] == position_[0] && center[2] == position_[2] ?
			glm::vec3{1.f, 0.f, 0.f} :
			glm::normalize(glm::cross(front_, glm::vec3{0.f, 1.f, 0.f}));
		up_ = glm::cross(right_, front_);
	}
	
	auto camera::orbit(glm::vec3 center, glm::vec2 delta) noexcept ->void {
		position_ -= center;
		auto R1 = glm::rotate(glm::mat4{1}, delta.x, glm::vec3{0.f, 1.f, 0.f});
		auto R2 = glm::rotate(glm::mat4{1}, delta.y, right_);
		auto R = R2 * R1;
		position_ = R * glm::vec4{position_, 1.f};
		front_ = R * glm::vec4{front_, 1.f};
		right_ = R * glm::vec4{right_, 1.f};
		up_ = R * glm::vec4{up_, 1.f};
		position_ += center;
	}

	auto camera::drift(glm::vec2 delta) noexcept -> void {
		auto R = glm::rotate(glm::mat4{1}, delta.y, right_) * glm::rotate(glm::mat4{1}, delta.x, glm::vec3{0.f, 1.f, 0.f});
		front_ = R * glm::vec4{front_, 1.f};
		right_ = R * glm::vec4{right_, 1.f};
		up_ = R * glm::vec4{up_, 1.f};
	}

	auto camera::zoom(float a) noexcept ->void {
		if (zoomed_fovy_ + a > glm::radians(5.f) and zoomed_fovy_ + a < glm::radians(120.f)) zoomed_fovy_ += a;
	}

	auto camera::aspect_ratio() const noexcept -> float {
		return aspect_ratio_;
	}

	auto camera::fovy() const noexcept -> float {
		return fovy_;
	}
	
	auto camera::near_distance() const noexcept -> float {
		return near_;
	}
	
	auto camera::far_distance() const noexcept -> float {
		return far_;
	}

	auto camera::position() const noexcept -> glm::vec3 {
		return position_;
	}

	auto camera::move_by(glm::vec3 distance) noexcept ->void {
		position_ += distance;
	}

	auto camera::move_to(glm::vec3 destination) noexcept ->void {
		position_ = destination;
	}

	auto camera::set_aspect_ratio(float aspect_ratio) noexcept -> void {
		aspect_ratio_ = aspect_ratio;
	}

	auto camera::set_near(float new_near) noexcept ->void {
		near_ = new_near;
	}
	
	auto camera::set_far(float new_far) noexcept ->void {
		far_ = new_far;
	}

	auto camera::set_fovy(float new_fovy) noexcept ->void {
		fovy_ = new_fovy;
		zoomed_fovy_ = fovy_;
	}

	auto camera::front() const noexcept ->glm::vec3 {
		return front_;
	}

	auto camera::up() const noexcept ->glm::vec3 {
		return up_;
	}

	auto camera::right() const noexcept ->glm::vec3 {
		return right_;
	}
}