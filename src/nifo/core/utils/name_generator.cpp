#include <nifo/core/utils/name_generator.h>

namespace nifo {
	auto name_generator::get(std::string prefix) -> std::string {
		auto& index = indices[prefix];
		if (index != 0) prefix = fmt::format("{}-{}", prefix, index);
		++index;
		return prefix;
	}
}
