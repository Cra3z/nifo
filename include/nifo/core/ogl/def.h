#pragma once
#include "config.h"
#include "../utils/core.h"
#include "../utils/meta.h"

namespace nifo {
	using float_t = GLfloat;
	using double_t = GLdouble;
	using byte = GLbyte;
	using int_t = GLint;
	using short_t = GLshort;
	using ssize_t = GLsizei;
	using intptr_t = GLintptr;
	using ubyte = GLubyte;
	using uint_t = GLuint;
	using ushort_t = GLushort;
}

namespace nifo {
	enum class primitive : GLenum {
		points = GL_POINTS,
		lines = GL_LINES,
		line_strip = GL_LINE_STRIP,
		line_loop = GL_LINE_LOOP,
		triangles = GL_TRIANGLES,
		triangle_strip = GL_TRIANGLE_STRIP,
		triangle_fan = GL_TRIANGLE_FAN
	};

	enum class polygon_mode : GLenum {
		fill = GL_FILL,
		point = GL_POINT,
		line = GL_LINE
	};

	enum class face_direction : GLenum {
		front = GL_FRONT,
		back = GL_BACK,
		front_and_back = GL_FRONT_AND_BACK
	};

	enum class buffer_type : GLenum {
		array_buffer = GL_ARRAY_BUFFER,
		atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
		copy_read_buffer = GL_COPY_READ_BUFFER,
		copy_write_buffer = GL_COPY_WRITE_BUFFER,
		dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
		draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
		element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
		pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
		pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
		query_buffer = GL_QUERY_BUFFER,
		shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
		texture_buffer = GL_TEXTURE_BUFFER,
		transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
		uniform_buffer = GL_UNIFORM_BUFFER
	};

	enum class buffer_usage : GLenum {
		static_draw = GL_STATIC_DRAW,
		static_read = GL_STATIC_READ,
		static_copy = GL_STATIC_COPY,
		stream_draw = GL_STREAM_DRAW,
		stream_read = GL_STREAM_READ,
		stream_copy = GL_STREAM_COPY,
		dynamic_draw = GL_DYNAMIC_DRAW,
		dynamic_read = GL_DYNAMIC_READ,
		dynamic_copy = GL_DYNAMIC_COPY
	};

	enum class frame_attachment_mask : GLbitfield {
		color = GL_COLOR_BUFFER_BIT,
		depth = GL_DEPTH_BUFFER_BIT,
		stencil = GL_STENCIL_BUFFER_BIT,
		all = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
	};

	/**
	 * @param lhs 左操作数
	 * @param rhs 右操作数
	 * @return 值为@p lhs 与@p rhs 底层类型的按位或的`frame_attachment_mask`
	 */
	constexpr auto operator| (const frame_attachment_mask& lhs, const frame_attachment_mask& rhs) noexcept ->frame_attachment_mask {
		return static_cast<frame_attachment_mask>(to_underlying(lhs) | to_underlying(rhs));
	}

	enum class channel : GLenum {
		grey = GL_RED,
		rgb = GL_RGB,
		rgba = GL_RGBA,
		depth = GL_DEPTH_COMPONENT,
		stencil = GL_STENCIL_INDEX
	};

	enum class texel_format : GLenum {
		automatic = 0,
		grey8 = GL_R8,
		grey8i = GL_R8I,
		grey8u = GL_R8UI,
		grey16 = GL_R16,
		grey16i = GL_R16I,
		grey16u = GL_R16UI,
		grey32i = GL_R32I,
		grey32u = GL_R32UI,
		grey16f = GL_R16F,
		grey32f = GL_R32F,
		rgb8 = GL_RGB8,
		rgb16 = GL_RGB16,
		rgb16f = GL_RGB16F,
		rgb32f = GL_RGB32F,
		rgba8 = GL_RGBA8,
		rgba16 = GL_RGBA16,
		rgba16f = GL_RGBA16F,
		rgba32f = GL_RGBA32F,
		srgb8 = GL_SRGB8,
		srgba8 = GL_SRGB8_ALPHA8,
		depth16 = GL_DEPTH_COMPONENT16,
		depth24 = GL_DEPTH_COMPONENT24,
		depth32 = GL_DEPTH_COMPONENT32,
		depth32f = GL_DEPTH_COMPONENT32F,
		stencil8 = GL_STENCIL_INDEX8,
		stencil16 = GL_STENCIL_INDEX16
	};

	enum class access_policy : GLenum {
		read_only = GL_READ_ONLY,
		write_only = GL_WRITE_ONLY,
		read_write = GL_READ_WRITE
	};

	enum class data_type {
		Int,
		Uint,
		Float,
		Double,

		Ivec2,
		Uvec2,
		Vec2,
		Dvec2,

		Vec3,
		Uvec3,
		Ivec3,
		Dvec3,

		Vec4,
		Uvec4,
		Ivec4,
		Dvec4
	};

	constexpr auto operator| (const access_policy& lhs, const access_policy& rhs) noexcept ->access_policy {
		if (lhs == rhs) return lhs;
		else return static_cast<access_policy>(GL_READ_WRITE);
	}
}