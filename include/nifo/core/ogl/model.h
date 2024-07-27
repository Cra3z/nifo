#pragma once
#include <filesystem>
#include "mesh.h"
#include "../utils/loop.h"

namespace nifo {

	namespace detail {
		struct model_config {
			int   forward_axis;
			int   up_axis;
			float scale;
			bool  flip_uv;

			friend auto operator== (const model_config& lhs, const model_config& rhs) ->bool = default;

			friend auto operator<=>(const model_config& lhs, const model_config& rhs) = default;
		};
	}

	struct model_load_information : detail::model_config {
		std::filesystem::path file;

		friend auto operator== (const model_load_information& lhs, const model_load_information& rhs) ->bool = default;

		friend auto operator<=>(const model_load_information& lhs, const model_load_information& rhs) = default;
	};

	class model {
	public:

		model() = default;

		explicit model(std::vector<mesh> meshes);

		explicit model(const model_load_information& preset, assets_loader<shader_part>& shader_manager, std::map<std::string, material>& material_pool, context_setter& context_setter);

		model(const model& other) = default;

		model(model&& other) noexcept;

		virtual ~model() = default;

		auto operator= (const model& other) ->model& = default;

		auto operator= (model&& other) noexcept ->model&;

		[[nodiscard]]
		auto meshes() const noexcept ->const std::vector<mesh>&;

		[[nodiscard]]
		auto vertex_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto index_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto face_count() const noexcept ->std::size_t;

		[[nodiscard]]
		auto mesh_count() const noexcept ->std::size_t;

		auto swap(model& other) noexcept ->void;

		friend auto swap(model& lhs, model& rhs) noexcept ->void {
			lhs.swap(rhs);
		}
	private:
		std::vector<mesh> meshes_;
		std::size_t vertex_count_{};
		std::size_t index_count_{};
		std::size_t face_count_{};
	};
}
