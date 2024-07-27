#include <atomic>
#include <deque>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <tbb/parallel_for.h>
#include <tbb/concurrent_queue.h>
#include <nifo/core/ogl/model.h>
#include <nifo/core/ogl/config.h>
#include <nifo/core/utils/loop.h>
#include <nifo/core/utils/lazy_evaluated.h>

namespace nifo {

	namespace { // TUL

		using mesh_information = std::tuple<std::vector<mesh::vertex_layout>, std::vector<unsigned int>, std::size_t, std::string, material>;

		[[nodiscard]]
		auto get_material_map_file_(const std::filesystem::path& dir, aiMaterial* mat, aiTextureType type) ->std::filesystem::path {
			// only take one
			aiString filename;
			mat->GetTexture(type, 0, &filename);
			return std::filesystem::absolute(dir / filename.C_Str());
		}

		auto get_lambert_material_attribute_(aiMaterial* in, material& out) ->void {
			{
				aiColor3D Kd{1.f};
				in->Get(AI_MATKEY_COLOR_DIFFUSE, Kd);
				out.attributes["diffuse_color"] = glm::vec4{Kd.r, Kd.g, Kd.b, 1.f};
			}
		}

		auto get_phong_material_attribute_(aiMaterial* in, material& out) ->void {
			{
				float Ns{};
				in->Get(AI_MATKEY_SHININESS, Ns);
				out.attributes["shininess"] = Ns * 128.f / 1000.f;
			}

			{
				float S = 1.f;
				in->Get(AI_MATKEY_SHININESS_STRENGTH, S);
				out.attributes["specular_intensity"] = S;
			}

			{
				float d = 1.f;
				in->Get(AI_MATKEY_OPACITY, d);
				out.attributes["opacity"] = d;
			}

			{
				aiColor3D Ka{1.f};
				in->Get(AI_MATKEY_COLOR_AMBIENT, Ka);
				out.attributes["ambient_color"] = glm::vec4{Ka.r, Ka.g, Ka.b, 1.f};
			}

			{
				aiColor3D Kd{1.f};
				in->Get(AI_MATKEY_COLOR_DIFFUSE, Kd);
				out.attributes["diffuse_color"] = glm::vec4{Kd.r, Kd.g, Kd.b, 1.f};
			}

			{
				aiColor3D Ks{1.f};
				in->Get(AI_MATKEY_COLOR_SPECULAR, Ks);
				out.attributes["specular_color"] = glm::vec4{Ks.r, Ks.g, Ks.b, 1.f};
			}

			{
				aiColor3D Ke{};
				in->Get(AI_MATKEY_COLOR_EMISSIVE, Ke);
				out.attributes["emissive_color"] = glm::vec4{Ke.r, Ke.g, Ke.b, 1.f};
			}
		}

		auto get_pbr_material_attribute_(aiMaterial* in, material& out) ->void; // TODO: 待获取PBR材质参数

		[[nodiscard]]
		auto get_mesh_info_(bool height_as_normal, const model_load_information& preset, const std::filesystem::path& dir, const aiScene* scene, aiMesh* ai_mesh) ->mesh_information {
			std::vector<mesh::vertex_layout> vertices;
			std::vector<unsigned int> indices;
			for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
				mesh::vertex_layout vtx{};
				vtx.position.x = preset.scale * ai_mesh->mVertices[i].x;
				vtx.position.y = preset.scale * ai_mesh->mVertices[i].y;
				vtx.position.z = preset.scale * ai_mesh->mVertices[i].z;

				vtx.normal.x = ai_mesh->mNormals[i].x;
				vtx.normal.y = ai_mesh->mNormals[i].y;
				vtx.normal.z = ai_mesh->mNormals[i].z;
				if (ai_mesh->mTextureCoords[0]) {
					vtx.uv.x = ai_mesh->mTextureCoords[0][i].x;
					vtx.uv.y = ai_mesh->mTextureCoords[0][i].y;
				}
				vertices.push_back(vtx);
			}
			for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
				aiFace face = ai_mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; ++j) {
					indices.push_back(face.mIndices[j]);
				}
			}

			aiMaterial* mat = scene->mMaterials[ai_mesh->mMaterialIndex];
			material mat_{};
			if (int mode; mat->Get(AI_MATKEY_SHADING_MODEL, mode) == AI_SUCCESS) {
				switch (mode) {
				case aiShadingMode_Phong:
					mat_.mode = shading_mode::phong;
					break;
				case aiShadingMode_Blinn:
					mat_.mode = shading_mode::blinn_phong;
					break;
				case aiShadingMode_PBR_BRDF:
					mat_.mode = shading_mode::pbr;
					break;
				default:
					mat_.mode = shading_mode::lambert;
				}
			}
			std::string name{"Unnamed Material"};
			if (aiString mat_name; mat->Get(AI_MATKEY_NAME, mat_name) == AI_SUCCESS) {
				name = mat_name.C_Str();
			}

			switch (mat_.mode) {
			using enum shading_mode;
			case lambert:
				get_lambert_material_attribute_(mat, mat_);
				break;
			case phong:
				[[fallthrough]];
			case blinn_phong:
				get_phong_material_attribute_(mat, mat_);
				break;
			default:
				; // TODO: 支持更多材质类型
			}

			using enum texture_mapping;
			for (auto t : {
				diffuse, normals, ambient_occlusion, metalness, roughness, height, specular, shininess, ambient, opacity
			}) {
				if (height_as_normal and t == height) t = normals;
				if (mat->GetTextureCount(static_cast<aiTextureType>(to_underlying(t)))) {
					mat_.attributes[fmt::format("{}", t)] = get_material_map_file_(dir, mat, static_cast<aiTextureType>(to_underlying(t)));
				}
				else if (t == diffuse) {
					mat_.attributes[fmt::format("{}", t)] = ASSETS_DIR"textures/builtin/default-diffuse.png";
				}
				else if (t == normals) {
					mat_.attributes[fmt::format("{}", t)] = ASSETS_DIR"textures/builtin/default-normal.png";
				}
			}
			return {std::move(vertices), std::move(indices), ai_mesh->mNumFaces, std::move(name), std::move(mat_)};
		}

		auto get_material_shaders_(material& mat_, assets_loader<shader_part>& shader_manager) ->void {
			switch (mat_.mode) {
				using enum shading_mode;
			case phong:
				mat_.shader = shader_program{
					shader_manager.load(ASSETS_DIR"shaders/builtin/phong.vert"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/phong.geom"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/phong.frag")
				};
				break;
			case blinn_phong:
				mat_.shader = shader_program{
					shader_manager.load(ASSETS_DIR"shaders/builtin/phong.vert"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/phong.geom"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/blinn_phong.frag")
				};
				break;
			case pbr:
				mat_.shader = shader_program{
					shader_manager.load(ASSETS_DIR"shaders/builtin/pbr.vert"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/pbr.geom"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/pbr.frag")
				};
				break;
			default: // TODO: support more shading modes
				mat_.shader = shader_program{
					shader_manager.load(ASSETS_DIR"shaders/builtin/half_lambert.vert"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/half_lambert.geom"),
					shader_manager.load(ASSETS_DIR"shaders/builtin/half_lambert.frag")
				};
				break;
			}
		}

		auto parallel_travel_(bool height_as_normal, const model_load_information& preset, const std::filesystem::path& dir, const aiScene* scene, std::vector<mesh>& meshes, assets_loader<shader_part>& shader_manager, std::unordered_map<std::string, material>& material_pool, context_setter& context_setter_) ->std::array<std::size_t, 3> {
			std::array<std::size_t, 3> result{};
			std::deque<aiNode*> nodes_{scene->mRootNode};
			std::deque<aiMesh*> mesh_queue;
			while (not nodes_.empty()) {
				auto front = nodes_.front();
				nodes_.pop_front();
				std::ranges::transform(
					std::views::iota(unsigned{}, front->mNumMeshes),
					std::back_inserter(mesh_queue),
					[&] (unsigned i) { return scene->mMeshes[front->mMeshes[i]]; }
				);
				std::ranges::copy_n(front->mChildren, front->mNumChildren, std::back_inserter(nodes_));
			}
			tbb::concurrent_queue<mesh_information> mesh_info_queue;
			tbb::parallel_for(tbb::blocked_range{std::size_t{}, mesh_queue.size()}, [&] (tbb::blocked_range<std::size_t> blk) {
				for (auto i = blk.begin(); i != blk.end();) {
					mesh_info_queue.push(get_mesh_info_(height_as_normal, preset, dir, scene, mesh_queue[i++]));
				}
			});
			loop _{[&] (const loop_controller& controller) {
				if (mesh_info_queue.empty()) return controller.request_stop();
				mesh_information mesh_info;
				for (bool available = false; not available;) {
					available = mesh_info_queue.try_pop(mesh_info);
				}
				context_setter_.make_current();
				auto fn = std::bind_front(&context_setter::done_current, std::ref(context_setter_));
				[[maybe_unused]] scope_exit guard{fn};
				auto& [vertices, indices, face_count_of_the_mesh_, mat_name, mat_] = mesh_info;
				auto it = material_pool.find(mat_name);
				if (it == material_pool.end()) {
					get_material_shaders_(mat_, shader_manager);
					it = material_pool.try_emplace(mat_name, std::move(mat_)).first;
				}
				else if (mat_ != it->second) {
					get_material_shaders_(mat_, shader_manager);
					if (mat_ != it->second) {
						mat_name = material::name_generator.get(mat_name);
						it = material_pool.try_emplace(mat_name, std::move(mat_)).first;
					}
				}
				result[0] += face_count_of_the_mesh_;
				result[1] += vertices.size();
				result[2] += indices.size();
				meshes.emplace_back(std::move(vertices), std::move(indices), face_count_of_the_mesh_, it->second);
			}};
			return result;
		}

		auto sync_travel_(
			bool height_as_normal,
			const model_load_information& preset,
			const std::filesystem::path& dir,
			const aiScene* scene,
			std::vector<mesh>& meshes,
			assets_loader<shader_part>& shader_manager,
			std::map<std::string, material>& material_pool,
			context_setter& context_setter
		) ->std::array<std::size_t, 3> {
			std::array<std::size_t, 3> result{};
			std::deque<aiNode*> nodes_{scene->mRootNode};
			loop _{[&] (const loop_controller& controller) {
				if (nodes_.empty()) return controller.request_stop();

				auto front = nodes_.front();
				nodes_.pop_front();
				std::ranges::copy_n(front->mChildren, front->mNumChildren, std::back_inserter(nodes_));

				context_setter.make_current();
				[[maybe_unused]] scope_exit guard{std::bind_front(&context_setter::done_current, std::ref(context_setter))};

				for (unsigned i = 0; i < front->mNumMeshes; ++i) {
					auto [vertices, indices, face_count_of_the_mesh_, mat_name, mat_] = get_mesh_info_(height_as_normal, preset, dir, scene, scene->mMeshes[front->mMeshes[i]]);
					auto [it, ok] = material_pool.try_emplace(mat_name, lazy_evaluated{[&] {
						get_material_shaders_(mat_, shader_manager);
						return std::move(mat_);
					}});
					if (not ok) {
						get_material_shaders_(mat_, shader_manager);
						if (it->second != mat_)
							it = material_pool.try_emplace(material::name_generator.get(mat_name), std::move(mat_)).first;
					}
					result[0] += face_count_of_the_mesh_;
					result[1] += vertices.size();
					result[2] += indices.size();
					meshes.emplace_back(std::move(vertices), std::move(indices), face_count_of_the_mesh_, it->second);
				}
			}};
			return result;
		}
	}

	model::model(std::vector<mesh> meshes) : meshes_(std::move(meshes)) {
		for (const auto& mesh : meshes_) {
			vertex_count_ += mesh.vertex_count();
			index_count_ += mesh.index_count();
			face_count_ += mesh.face_count();
		}
	}

	model::model(const model_load_information& preset, assets_loader<shader_part>& shader_manager, std::map<std::string, material>& material_pool, context_setter& context_setter) {
		const auto& path = preset.file;
		Assimp::Importer importer;
		auto ai_process = aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_Triangulate;
		if (preset.flip_uv) ai_process |= aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(path.string(), ai_process);
		if (
			scene == nullptr or
			(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) or
			not scene->mRootNode
		) throw bad_resource_acquisition{importer.GetErrorString()};
		auto counts = sync_travel_(path.extension().string() == ".obj", preset, path.parent_path(), scene, meshes_, shader_manager, material_pool, context_setter);
		face_count_ =   counts[0];
		vertex_count_ = counts[1];
		index_count_ =  counts[2];
	}

	model::model(model&& other) noexcept :
		meshes_(std::move(other.meshes_)),
		vertex_count_(std::exchange(other.vertex_count_, {})),
		index_count_(std::exchange(other.index_count_, {})),
		face_count_(std::exchange(other.face_count_, {})) {}

	auto model::operator=(model&& other) noexcept -> model& {
		model{std::move(other)}.swap(*this);
		return *this;
	}

	auto model::meshes() const noexcept ->const std::vector<mesh>& {
		return meshes_;
	}

	auto model::vertex_count() const noexcept -> std::size_t {
		return vertex_count_;
	}

	auto model::index_count() const noexcept -> std::size_t {
		return index_count_;
	}

	auto model::face_count() const noexcept -> std::size_t {
		return face_count_;
	}

	auto model::mesh_count() const noexcept -> std::size_t {
		return meshes_.size();
	}

	auto model::swap(model& other) noexcept -> void {
		std::ranges::swap(meshes_, other.meshes_);
		std::ranges::swap(vertex_count_, other.vertex_count_);
		std::ranges::swap(index_count_, other.index_count_);
		std::ranges::swap(face_count_, other.face_count_);
	}
}
