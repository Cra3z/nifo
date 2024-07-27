#pragma once
#include <unordered_map>
#include "diagnostics.h"

#ifdef NIFO_GUI_USE_QT

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Compatibility>

#define NIFO_GL_FUNCTIONS QOpenGLFunctions_4_5_Compatibility
#define NIFO_GL_FUNCTIONS_SCOPE NIFO_GL_FUNCTIONS::
#define NIFO_GL_FUNCTIONS_POINTER_PREFIX NIFO_GL_FUNCTIONS::*

#endif

namespace nifo::detail {

#ifdef NIFO_GUI_USE_QT
	inline QOpenGLFunctions_4_5_Compatibility* caller = nullptr;
#endif

	extern std::unordered_map<GLenum, const char*> gl_errors;

	auto format_error(const char* gl_func_name, const char* file_name, std::size_t line, GLenum errc) ->std::string ;

	template<typename R, typename... Args>
	auto invoke(const char* gl_func_name, const char* file_name, std::size_t line, R(NIFO_GL_FUNCTIONS_POINTER_PREFIX gl_func)(Args...), std::type_identity_t<Args>... args) {
#ifdef NIFO_GUI_USE_QT
		std::string info;
		nifo_expect(caller != nullptr);
		if constexpr (std::is_void_v<R>) {
			(caller->*gl_func)(args...);
			GLenum errc;
			while ((errc = caller->glGetError()) != GL_NO_ERROR) {
				info += format_error(gl_func_name, file_name, line, errc);
			}
			if (!info.empty()) {
				spdlog::error("{}", info);
				std::terminate();
			}
		}
		else {
			auto result = (caller->*gl_func)(args...);
			GLenum errc;
			while ((errc = caller->glGetError()) != GL_NO_ERROR) {
				info += format_error(gl_func_name, file_name, line, errc);
			}
			if (!info.empty()) throw opengl_api_error{info};
			return result;
		}
#endif
	}

	auto ogl_error_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param) ->void;
}

#define nifo_gl_invoke(gl_func, ...) ::nifo::detail::invoke(#gl_func, __FILE__, __LINE__, &NIFO_GL_FUNCTIONS_SCOPE gl_func __VA_OPT__(,) __VA_ARGS__)