#include <nifo/core/ogl/config.h>
#include <nifo/core/utils/core.h>
#include <map>

namespace nifo::detail {
	std::unordered_map<GLenum, const char*> gl_errors {
			{GL_NO_ERROR,                      "No Error"},
			{GL_INVALID_ENUM,                  "Invalid Enum"},
			{GL_INVALID_VALUE,                 "Invalid Value"},
			{GL_INVALID_OPERATION,             "Invalid Operation"},
			{GL_INVALID_FRAMEBUFFER_OPERATION, "Invalid Frame Buffer Operation"},
			{GL_OUT_OF_MEMORY,                 "Out Of Memory"}
	};

	auto format_error(const char* gl_func_name, const char* file_name, std::size_t line, GLenum errc) -> std::string {
		return fmt::format(R"(at "{}:{}" OpenGL Function `{}`: {})", std::filesystem::absolute(file_name), line, gl_func_name, gl_errors[errc]);
	}

	auto ogl_error_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param) ->void {
		if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
		static std::map<GLenum, std::string> src_dict{
			{GL_DEBUG_SOURCE_API,             "api"},
			{GL_DEBUG_SOURCE_WINDOW_SYSTEM,   "window system"},
			{GL_DEBUG_SOURCE_SHADER_COMPILER, "shader compiler"},
			{GL_DEBUG_SOURCE_THIRD_PARTY,     "third party"},
			{GL_DEBUG_SOURCE_APPLICATION,     "application"},
			{GL_DEBUG_SOURCE_OTHER,           "other"},
		};

		static std::map<GLenum, std::string> type_dict{
			{GL_DEBUG_TYPE_ERROR,               "fatal error"},
			{GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "deprecated behavior"},
			{GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,  "undefined behavior"},
			{GL_DEBUG_TYPE_PORTABILITY,         "portability"},
			{GL_DEBUG_TYPE_PERFORMANCE,         "performance"},
			{GL_DEBUG_TYPE_MARKER,              "marker"},
			{GL_DEBUG_TYPE_OTHER,               "other"},
		};

		static std::map<GLenum, std::string> severity_dict {
			{GL_DEBUG_SEVERITY_NOTIFICATION, "notification"},
			{GL_DEBUG_SEVERITY_LOW         , "low"},
			{GL_DEBUG_SEVERITY_MEDIUM      , "medium"},
			{GL_DEBUG_SEVERITY_HIGH        , "high"},
		};
		if (type == GL_DEBUG_TYPE_ERROR or type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
			spdlog::error("[id: {}] [{}] [severity: {}] [source: {}] {}", id, type_dict[type], severity_dict[severity], src_dict[source], message);
		}
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
			spdlog::warn("[id: {}] [{}] [severity: {}] [source: {}] {}", id, type_dict[type], severity_dict[severity], src_dict[source], message);
		}
		else {
			spdlog::info("[id: {}] [{}] [severity: {}] [source: {}] {}", id, type_dict[type], severity_dict[severity], src_dict[source], message);
		}
	}
}
