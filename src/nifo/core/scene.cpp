#include <ranges>
#include <nifo/core/scene.h>
#include <../../../include/nifo/core/ogl/diagnostics.h>
#include <nifo/core/ecs/assets_loader.h>

namespace nifo {

	std::map<std::string, std::reference_wrapper<scene>> scene_dictionary;

	scene::scene(std::string_view name) : name(name) {
		root = std::unique_ptr<node>{new node{*this, components::root{}}};
	}

	scene::scene() : scene("New Scene") {}

	auto scene::get_node(std::string_view name) -> node* {
		for (const auto& [id_, name_comp] : registry.view<components::name>().each()) {
			if (name_comp.value == name) return get_node(id_);
		}
		return nullptr;
	}

	auto scene::get_node(entt::entity id) -> node* {
		if (auto it = entities.find(id); it != entities.end()) [[likely]] {
			return &it->second.get();
		}
		return nullptr;
	}

	auto scene_manager::create_new_scene(std::string_view scene_name) -> void {
		scenes_.emplace_back(scene_name);
	}

	auto scene_manager::current_scene() noexcept -> scene& {
		return scenes_.back();
	}

	auto scene_manager::clear() noexcept -> void {
		scenes_.clear();
	}

	auto node_parent(node& parent) noexcept -> detail::node_parent {
		return {&parent};
	}

	node::node(entt::entity id) noexcept : id_(id) {} // NOLINT

	node::node(std::string_view name, class scene& scene_) : node(scene_.registry.create()) {
		this->scene_ = &scene_;
		scene_.entities.try_emplace(id_, std::ref(*this));
		add_component<components::name>(scene_.name_generator_.get(name.data()));
		add_component<components::transform>(components::transform{.owner = std::pair{std::ref(scene_), id_}});
	}

	node::node(nifo::scene& scene_) : node("Node", scene_) {}

	node::node(class scene& scene_, components::root) : node(scene_.registry.create()) {
		this->scene_ = &scene_;
		scene_.entities.try_emplace(id_, std::ref(*this));
		add_component<components::name>(scene_.name);
		add_component<components::root>();
	}

	node::node(detail::node_parent parent) : node(parent.ptr->scene()) {
		set_parent_impl_(parent.ptr);
	}

	node::node(std::string_view name, detail::node_parent parent) : node(name, parent.ptr->scene()) {
		set_parent_impl_(parent.ptr);
	}

	node::node(std::shared_ptr<model> model, detail::node_parent parent) : node(parent) {
		add_component<components::model>(std::move(model));
	}

	node::node(std::shared_ptr<model> model, std::string_view name, detail::node_parent parent) : node(name, parent) {
		add_component<components::model>(std::move(model));
	}

	node::node(const node& other) : node(other.scene()) {
		copy_all_components_of(other);
		for (const auto& child : other.children_) {
			add_child(child->copy_impl_());
		}
		set_parent_impl_(other.parent_);
	}

	node::~node() {
		set_parent_impl_(nullptr);
		for (const auto& child : children_) child->parent_ = nullptr;
		children_.clear();
		scene_->registry.destroy(id_);
	}

	auto node::copy() const -> node* {
		auto copied = copy_impl_().release();
		parent_->add_child(copied);
		return copied;
	}

	auto node::copy_all_components_of(const node& other) -> void {
		nifo_expect(scene_ == other.scene_); // the node can't copy to other nodes in another different scene
		for (const auto& [comp_type, copy_pir] : other.components_) {
			const auto& [comp_ptr, copy_fn] = copy_pir;
			components_[comp_type] = {copy_fn(comp_ptr, *scene_, id_), copy_fn};
		}
	}

	auto node::parent() const noexcept ->node* {
		return parent_;
	}

	auto node::children() const noexcept ->const child_node_array& {
		return children_;
	}

	auto node::child_count() const noexcept -> std::size_t {
		return children_.size();
	}

	auto node::find_child(std::string_view name) const noexcept ->node* {
		auto it = std::ranges::find_if(children_, [name](const std::unique_ptr<node>& child) noexcept ->bool {
			return child->get_components<components::name>().value == name;
		});
		if (it == children_.end()) return nullptr;
		return it->get();
	}

	auto node::find_children_if(move_only_function<bool(const node*)> pred) const -> std::vector<node*> {
		std::vector<node*> result;
		for (const auto& child : children_) {
			if (pred(child.get())) result.emplace_back(child.get());
		}
		return result;
	}

	auto node::id() const noexcept -> entt::entity {
		return id_;
	}

	auto node::scene() const noexcept -> nifo::scene& {
		return *scene_;
	}

	auto node::name() const noexcept -> std::string_view {
		return get_components<components::name>().value;
	}

	auto node::add_child(node* child) -> void {
		add_child(std::unique_ptr<node>{child});
	}

	auto node::add_child(std::unique_ptr<node> child) -> void {
		nifo_expect(child != nullptr); // "child can't be null"
		child->set_parent(this);
		void(child.release());
	}

	auto node::remove_child(node* child) noexcept -> void {
		std::erase_if(children_, [child] (const std::unique_ptr<node>& child_) noexcept ->bool {
			return child_.get() == child;
		});
	}

	auto node::remove_children() noexcept -> void {
		children_.clear();
	}

	auto node::remove_self() noexcept -> void {
		delete this;
	}

	auto node::set_parent(node* parent) -> void {
		if (parent) nifo_expect(parent->scene_ == scene_); // parent and child must be in the same scene
		set_parent_impl_(parent);
	}

	auto node::rename(std::string_view new_name) -> void {
		get_components<components::name>().value = new_name;
	}

	auto node::components_type_info() const -> std::vector<entt::type_info> {
		std::vector<entt::type_info> result;
		std::ranges::copy(components_ | std::views::keys, std::back_inserter(result));
		return result;
	}

	auto node::transform() noexcept -> components::transform& {
		return get_components<components::transform>();
	}

	auto node::transform() const noexcept ->const components::transform& {
		return get_components<components::transform>();
	}

	auto node::is_root() const noexcept -> bool {
		return has_all_components_of<components::root>();
	}

	auto node::copy_impl_() const -> std::unique_ptr<node> {
		auto result = std::unique_ptr<node>(new node{scene()});
		result->copy_all_components_of(*this);
		for (const auto& child : children_) result->add_child(child->copy_impl_());
		return result;
	}

	auto node::set_parent_impl_(node* parent) -> void {
		if (parent_ == parent) return;
		if (parent_) {
			auto it = std::ranges::find_if(parent_->children_, [this](const std::unique_ptr<node>& child) noexcept ->bool {
				return child.get() == this;
			});
			void(it->release());
			parent_->children_.erase(it);
		}
		parent_ = parent;
		if (parent) parent->children_.emplace_back(this);
	}
}
