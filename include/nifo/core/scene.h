#pragma once
#include <vector>
#include <memory>
#include <stack>
#include <unordered_map>
#include "ogl/camera.h"
#include "ecs/components/transform.h"
#include "ecs/components/tags.h"
#include "ecs/components/model.h"
#include "utils/name_generator.h"
#include "builtin/grid.h"

template<>
struct std::hash<entt::type_info> {
	auto operator() (const entt::type_info& type_info) const noexcept {
		return type_info.hash();
	}
};

namespace nifo {

	class scene {
		friend class node;
	public:

		scene(std::string_view name);

		scene();

		scene(const scene&) = delete;

		~scene() = default;

		auto operator= (const scene& other) ->scene& = delete;

		[[nodiscard]]
		auto get_node(std::string_view name) ->node*;

		[[nodiscard]]
		auto get_node(entt::entity id) ->node*;
	public:
		std::string name;
		entt::registry registry;
		std::unordered_map<entt::entity, std::reference_wrapper<node>> entities;
		name_generator name_generator_;
		std::unique_ptr<node> root;
		std::unique_ptr<builtin::grid> grid;
		camera editor_camera{glm::vec3{100.f, 100.f, 100.f}, 1.f, std::numbers::pi_v<float> / 3, 0.5f, 1500.f, {}};
	};

	extern std::map<std::string, std::reference_wrapper<scene>> scene_dictionary;

	class scene_manager {
	public:

		scene_manager() = default;

		scene_manager(const scene&) = delete;

		auto operator= (const scene&) ->scene& = delete;

		auto create_new_scene(std::string_view scene_name) ->void;

		[[nodiscard]]
		auto current_scene() noexcept ->scene&;

		auto clear() noexcept ->void;
	private:
		std::list<scene> scenes_;
	};

	namespace detail {

		struct node_parent {
			node* ptr;
		};

		template<component Component>
		auto on_component_copy(void* comp_ptr, class scene& scene, entt::entity entity) ->void* {
			auto& comp = scene.registry.emplace_or_replace<Component>(entity, *static_cast<Component*>(comp_ptr));
			if constexpr (owner_aware_component<Component>) {
				comp.owner = std::pair{std::ref(scene), entity};
			}
			if constexpr (std::same_as<Component, components::name>) {
				comp.value.append("-copy");
			}
			if constexpr (requires (Component component) {component.refresh_cache();}) {
				comp.refresh_cache();
			}
			return &comp;
		}

		template<component Component>
		auto on_empty_component_copy(void*, class scene& scene, entt::entity entity) ->void* {
			scene.registry.emplace_or_replace<Component>(entity, Component{});
			return nullptr;
		}
	}

	[[nodiscard]]
	auto node_parent(node& parent) noexcept ->detail::node_parent;

	class node {

		friend class scene;

	public:

		using child_node_array = std::vector<std::unique_ptr<node>>;

	private:

		node(entt::entity id) noexcept; // NOLINT

		node(std::string_view name, scene& scene_); // NOLINT

		node(scene& scene_);

		node(scene& scene_, components::root);

	public:

		explicit node(detail::node_parent parent);

		explicit node(std::string_view name, detail::node_parent parent);

		explicit node(std::shared_ptr<model> model, detail::node_parent parent);

		explicit node(std::shared_ptr<model> model, std::string_view name, detail::node_parent parent);

		node(const node& other);

		virtual ~node();

		auto operator= (const node& other) ->node& = delete;

		[[nodiscard]]
		auto copy() const -> node*;

		auto copy_all_components_of(const node& other) ->void;

		[[nodiscard]]
		auto parent() const noexcept ->node*;

		[[nodiscard]]
		auto children() const noexcept ->const child_node_array&;

		[[nodiscard]]
		auto child_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto find_child(std::string_view name) const noexcept ->node*;

		[[nodiscard]]
		auto find_children_if(move_only_function<bool(const node*)> pred) const ->std::vector<node*>;

		[[nodiscard]]
		auto id() const noexcept ->entt::entity;

		[[nodiscard]]
		auto scene() const noexcept ->nifo::scene&;

		[[nodiscard]]
		auto name() const noexcept ->std::string_view;

		auto add_child(node* child) ->void;

		auto add_child(std::unique_ptr<node> child) ->void;

		auto remove_child(node* child) noexcept ->void;

		auto remove_children() noexcept ->void;

		auto remove_self() noexcept ->void;

		auto set_parent(node* parent) ->void;

		auto rename(std::string_view new_name) ->void;

		template<component Component, typename... Args> requires std::same_as<Component, std::remove_cvref_t<Component>> && std::constructible_from<Component, Args...>
		decltype(auto) add_component(Args&&... args) {
			if constexpr (std::is_empty_v<Component>) {
				scene_->registry.emplace<Component>(id_, std::forward<Args>(args)...);
				components_.try_emplace(
					entt::type_id<Component>(),
					static_cast<void*>(nullptr),
					&detail::on_empty_component_copy<Component>
				);
			}
			else {
				auto& result = scene_->registry.emplace<Component>(id_, std::forward<Args>(args)...);
				components_.try_emplace(
					entt::type_id<Component>(),
					static_cast<void*>(&result),
					&detail::on_component_copy<Component>
				);
				return result;
			}
		}

		template<component... Components> requires (... && std::same_as<Components, std::remove_cvref_t<Components>>)
		auto remove_components() ->void {
			scene_->registry.remove<Components...>(id_);
			(..., components_.erase(entt::type_id<Components>()));
		}

		template<component Component, std::invocable<Component&> Fn> requires std::same_as<Component, std::remove_cvref_t<Component>>
		decltype(auto) modify_component(Fn&& fn) {
			return scene_->registry.patch<Component>(std::forward<Fn>(fn));
		}

		template<component... Components> requires (... && std::same_as<Components, std::remove_cvref_t<Components>>)
		[[nodiscard]]
		decltype(auto) get_components() noexcept {
			return scene_->registry.get<Components...>(id_);
		}

		template<component... Components> requires (... && std::same_as<Components, std::remove_cvref_t<Components>>)
		[[nodiscard]]
		const auto& get_components() const noexcept {
			return scene_->registry.get<Components...>(id_);
		}

		template<component... Components> requires (... && std::same_as<Components, std::remove_cvref_t<Components>>)
		[[nodiscard]]
		auto has_all_components_of() const noexcept ->bool {
			return scene_->registry.all_of<Components...>(id_);
		}

		template<component... Components> requires (... && std::same_as<Components, std::remove_cvref_t<Components>>)
		[[nodiscard]]
		auto has_any_component_of() const noexcept ->bool {
			return scene_->registry.any_of<Components...>(id_);
		}

		[[nodiscard]]
		auto components_type_info() const ->std::vector<entt::type_info>;

		[[nodiscard]]
		auto transform() noexcept ->components::transform&;

		[[nodiscard]]
		auto transform() const noexcept ->const components::transform&;

		[[nodiscard]]
		auto is_root() const noexcept ->bool;
	private:

		[[nodiscard]]
		auto copy_impl_() const -> std::unique_ptr<node>;

		auto set_parent_impl_(node* parent) ->void;

	protected:
		entt::entity id_;
		nifo::scene* scene_;
		node* parent_ = nullptr;
		child_node_array children_;
		std::unordered_map<entt::type_info, std::pair<void*, void*(*)(void*, class scene&, entt::entity)>> components_;
	};

}
