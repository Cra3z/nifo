#pragma once
#include <memory>
#include <concepts>
#include <filesystem>
#include <map>
#include "../utils/meta.h"

namespace nifo {
	template<typename ResourceType, typename KeyType = std::filesystem::path, typename Alloc = std::allocator<ResourceType>> requires std::same_as<ResourceType, std::remove_cv_t<ResourceType>> and std::copyable<KeyType> and meta::less_than_comparable<KeyType>
	class assets_loader {
	public:
		using resource_type = ResourceType;
		using key_type = KeyType;
		using allocator_type = Alloc;
		using dictionary_type = std::map<key_type, std::weak_ptr<resource_type>>;
	public:

		assets_loader() = default;

		assets_loader(const assets_loader&) = delete;

		auto operator= (const assets_loader&) ->assets_loader& = delete;

		template<typename... Args> requires std::constructible_from<resource_type, key_type, Args...>
		[[nodiscard]]
		auto load(const key_type& file, Args&&... args) ->std::shared_ptr<resource_type> {
			if (not contains(file)) {
				auto result = std::allocate_shared<resource_type>(alloc_, file, std::forward<Args>(args)...);
				dict_.insert_or_assign(file, std::weak_ptr{result});
				return result;
			}
			return std::shared_ptr{dict_[file]};
		};

		[[nodiscard]]
		auto contains(const key_type& file) noexcept ->bool {
			auto it = dict_.find(file);
			return it != dict_.end() and not it->second.expired();
		}

		[[nodiscard]]
		auto get_allocator() const ->allocator_type {
			return alloc_;
		}
	private:
		dictionary_type dict_;
		allocator_type alloc_;
	};
}