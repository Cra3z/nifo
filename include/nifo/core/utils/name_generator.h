#pragma once
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include "meta.h"

namespace nifo {

	class name_generator {
	public:
		[[nodiscard]]
		auto get(std::string prefix) ->std::string;

	private:
		std::unordered_map<std::string, std::size_t> indices;
	};

}
