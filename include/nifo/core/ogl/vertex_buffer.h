#pragma once
#include <numeric>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "def.h"

namespace nifo {

	auto draw_arrays(primitive mode, int_t first, ssize_t count) ->void;
	
	auto draw_elements(primitive mode, ssize_t count, const void *indices = nullptr) ->void;

	template<buffer_type BufferType>
	class basic_buffer {
		friend class vertex_array;
	public:

		class mmap_deleter {
		public:

			mmap_deleter() = default;

			explicit mmap_deleter(basic_buffer& owner) noexcept : owner(&owner) {}

			mmap_deleter(const mmap_deleter&) = delete;

			mmap_deleter(mmap_deleter&& other) noexcept : owner(std::exchange(other.owner, {})) {}

			auto operator= (const mmap_deleter&) ->mmap_deleter& = delete;

			auto operator= (mmap_deleter&& other) noexcept ->mmap_deleter& {
				mmap_deleter{std::move(other)}.swap(*this);
				return *this;
			}

			auto swap(mmap_deleter& other) noexcept ->void {
				std::ranges::swap(owner, other.owner);
			}

			friend auto swap(mmap_deleter& lhs, mmap_deleter& rhs) noexcept ->void {
				lhs.swap(rhs);
			}

			auto operator() (void*) const noexcept ->void {
				if (owner) nifo_gl_invoke(glUnmapNamedBuffer, owner->id);
			}
		private:
			basic_buffer* owner = nullptr;
		};

		using unique_mmap_handle = std::unique_ptr<void, mmap_deleter>;

	public:

		basic_buffer() = default;
		
		explicit basic_buffer(uint_t id_) noexcept : id(id_) {}

		basic_buffer(uint_t id_, uint_t capacity, buffer_usage usage = buffer_usage::static_draw) : basic_buffer(id_) {
			nifo_gl_invoke(glNamedBufferData, id, capacity, nullptr, to_underlying(usage));
		}

		template<typename Rng> requires std::ranges::sized_range<Rng> && std::ranges::contiguous_range<Rng> && meta::trivially_copyable<std::ranges::range_value_t<Rng>>
		basic_buffer(uint_t id_, const Rng& rng, buffer_usage usage) : basic_buffer(id_) {
			this->from_range(rng, usage);
		}

		template<typename... Rngs> requires (... && std::ranges::sized_range<Rngs>) && (... && std::ranges::contiguous_range<Rngs>) && (... && meta::trivially_copyable<std::ranges::range_value_t<Rngs>>)
		basic_buffer(uint_t id_, const Rngs&... rngs, buffer_usage usage) : basic_buffer(id_) {
			this->from_ranges(rngs..., usage);
		}

		template<typename Rng> requires std::ranges::sized_range<Rng> && std::ranges::contiguous_range<Rng> && meta::trivially_copyable<std::ranges::range_value_t<Rng>>
		basic_buffer(uint_t id_, const std::vector<std::reference_wrapper<const Rng>>& rngs, buffer_usage usage) : basic_buffer(id_) {
			this->from_ranges(rngs, usage);
		}
		
		basic_buffer(const basic_buffer&) = delete;
		
		basic_buffer(basic_buffer&& other) noexcept : id(std::exchange(other.id, {})) {}
		
		~basic_buffer() {
			if (id) nifo_gl_invoke(glDeleteBuffers, 1, &id);
		}
		
		auto operator= (const basic_buffer&) ->basic_buffer& = delete;
		
		auto operator= (basic_buffer&& other) noexcept ->basic_buffer& {
			basic_buffer{std::move(other)}.swap(*this);
			return *this;
		}
		
		auto swap(basic_buffer& other) noexcept ->void {
			std::ranges::swap(id, other.id);
		}
		
		friend auto swap(basic_buffer& lhs, basic_buffer& rhs) noexcept ->void {
			lhs.swap(rhs);
		}
		
		auto bind() ->void {
			nifo_gl_invoke(glBindBuffer, to_underlying(BufferType), id);
		}
		
		auto unbind() ->void {
			nifo_gl_invoke(glBindBuffer, to_underlying(BufferType), 0);
		}

		auto bind_point(uint_t point) ->void requires (
			BufferType == buffer_type::uniform_buffer or
			BufferType == buffer_type::shader_storage_buffer or
			BufferType == buffer_type::atomic_counter_buffer or
			BufferType == buffer_type::transform_feedback_buffer) {
			nifo_gl_invoke(glBindBufferBase, to_underlying(BufferType), point, id);
		}

		[[nodiscard]]
		auto get_id() const noexcept ->uint_t {
			return id;
		}

		[[nodiscard]]
		auto size() const noexcept ->ssize_t {
			ssize_t size;
			nifo_gl_invoke(glGetNamedBufferParameteriv, id, GL_BUFFER_SIZE, &size);
			return size;
		}

		template<typename Rng> requires std::ranges::sized_range<Rng> && std::ranges::contiguous_range<Rng> && meta::trivially_copyable<std::ranges::range_value_t<Rng>>
		auto from_range(const Rng& rng, buffer_usage usage) ->void {
			nifo_gl_invoke(glNamedBufferData, id, std::ranges::size(rng) * sizeof(std::ranges::range_value_t<Rng>), std::ranges::data(rng), to_underlying(usage));
		}

		template<typename... Rngs> requires (... && std::ranges::sized_range<Rngs>) && (... && std::ranges::contiguous_range<Rngs>) && (... && meta::trivially_copyable<std::ranges::range_value_t<Rngs>>)
		auto from_ranges(const Rngs&... rngs, buffer_usage usage) ->void {
			nifo_gl_invoke(glNamedBufferData, id, (... + (std::ranges::size(rngs) * sizeof(std::ranges::range_value_t<Rngs>))), nullptr, to_underlying(usage));
			::GLsizeiptr offset = 0;
			(..., (nifo_gl_invoke(glNamedBufferSubData, id, offset, offset + std::ranges::size(rngs) * sizeof(std::ranges::range_value_t<Rngs>), std::ranges::data(rngs)), (void) offset += std::ranges::size(rngs) * sizeof(std::ranges::range_value_t<Rngs>)));
		}

		template<typename Rng> requires std::ranges::sized_range<Rng> && std::ranges::contiguous_range<Rng> && meta::trivially_copyable<std::ranges::range_value_t<Rng>>
		auto from_ranges(const std::vector<std::reference_wrapper<const Rng>>& rngs, buffer_usage usage) ->void {
			nifo_gl_invoke(
				glNamedBufferData,
				id,
				std::accumulate(
					rngs.begin(), rngs.end(), ::GLsizeiptr{},
					[](GLsizeiptr offset, const Rng& rng) ->::GLsizeiptr {
						return offset + std::ranges::size(rng) * sizeof(std::ranges::range_value_t<Rng>);
					}
				),
				nullptr,
				to_underlying(usage)
			);
			for (::GLsizeiptr offset = 0; const Rng& rng : rngs) {
				nifo_gl_invoke(glNamedBufferSubData, id, offset, std::ranges::size(rng) * sizeof(std::ranges::range_value_t<Rng>), std::ranges::data(rng));
				offset += std::ranges::size(rng) * sizeof(std::ranges::range_value_t<Rng>);
			}
		}

		template<typename Rng> requires requires (Rng& r) {std::ranges::size(r); std::ranges::data(r);}
		auto modify_data(intptr_t offset, ssize_t size, const Rng& rng) ->void {
			if (std::ranges::size(rng) * sizeof(std::ranges::range_value_t<Rng>) < size) throw std::invalid_argument{"the size of `rng` less than `size` divided by the size of value_type of `rng`"};
			nifo_gl_invoke(glNamedBufferSubData, id, offset, size, std::ranges::data(rng));
		}

		[[nodiscard]]
		auto mmap(access_policy pol = access_policy::read_write) ->unique_mmap_handle {
			if (mmapped()) throw runtime_error{"the buffer has been mapped"};
			return unique_mmap_handle{nifo_gl_invoke(glMapNamedBuffer, id, to_underlying(pol)), mmap_deleter{*this}};
		}

		auto mmapped() ->bool {
			int_t mapped_;
			nifo_gl_invoke(glGetNamedBufferParameteriv, id, GL_BUFFER_MAPPED, &mapped_);
			return mapped_;
		}

		template<typename Rng> requires requires (Rng& r) {std::ranges::size(r); std::ranges::data(r);}
		auto copy_to(intptr_t offset, ssize_t size, Rng& rng) ->void {
			if (std::ranges::size(rng) * sizeof(std::ranges::range_value_t<Rng>) < size) throw std::invalid_argument{"the size of `rng` less than `size` divided by the size of value_type of `rng`"};
			return nifo_gl_invoke(glGetNamedBufferSubData, id, offset, size, std::ranges::data(rng));
		}

	private:
		uint_t id{};
	};
	
	using vertex_buffer = basic_buffer<buffer_type::array_buffer>;
	using element_buffer = basic_buffer<buffer_type::element_array_buffer>;
	using uniform_buffer = basic_buffer<buffer_type::uniform_buffer>;
	using shader_storage_buffer = basic_buffer<buffer_type::shader_storage_buffer>;

	struct vertex_attribute_format {
		data_type type;
		ssize_t stride;
		uint_t relative_offset;
		bool normalized = false;
	};

	namespace detail {
		template<buffer_type BufferType>
		struct create_buffer_fn_ {
			auto operator() () noexcept ->basic_buffer<BufferType> {
				uint_t id_;
				nifo_gl_invoke(glCreateBuffers, 1, &id_);
				basic_buffer<BufferType> result{id_};
				return result;
			}

			auto operator() (uint_t capacity, buffer_usage usage = buffer_usage::static_draw) ->basic_buffer<BufferType> {
				uint_t id_;
				nifo_gl_invoke(glCreateBuffers, 1, &id_);
				basic_buffer<BufferType> result{id_, capacity, usage};
				return result;
			}

			template<typename Rng> requires std::ranges::sized_range<Rng> && std::ranges::contiguous_range<Rng> && meta::trivially_copyable<std::ranges::range_value_t<Rng>>
			auto operator() (const Rng& rng, buffer_usage usage = buffer_usage::static_draw) ->basic_buffer<BufferType> {
				uint_t id_;
				nifo_gl_invoke(glCreateBuffers, 1, &id_);
				basic_buffer<BufferType> result{id_, rng, usage};
				return result;
			}

			template<typename... Rngs> requires (... && std::ranges::sized_range<Rngs>) && (... && std::ranges::contiguous_range<Rngs>) && (... && meta::trivially_copyable<std::ranges::range_value_t<Rngs>>)
			auto operator() (const Rngs&... rngs, buffer_usage usage = buffer_usage::static_draw) ->basic_buffer<BufferType> {
				uint_t id_;
				nifo_gl_invoke(glCreateBuffers, 1, &id_);
				basic_buffer<BufferType> result{id_, rngs..., usage};
				return result;
			}

			template<typename Rng> requires std::ranges::sized_range<Rng> && std::ranges::contiguous_range<Rng> && meta::trivially_copyable<std::ranges::range_value_t<Rng>>
			auto operator() (const std::vector<std::reference_wrapper<const Rng>>& rngs, buffer_usage usage = buffer_usage::static_draw) ->basic_buffer<BufferType> {
				uint_t id_;
				nifo_gl_invoke(glCreateBuffers, 1, &id_);
				basic_buffer<BufferType> result{id_, rngs, usage};
				return result;
			}
		};
	}
	
	template<buffer_type BufferType>
	inline detail::create_buffer_fn_<BufferType> create_buffer{};
	
	inline detail::create_buffer_fn_<buffer_type::array_buffer> create_vertex_buffer{};
	
	inline detail::create_buffer_fn_<buffer_type::element_array_buffer> create_element_buffer{};

	inline detail::create_buffer_fn_<buffer_type::uniform_buffer> create_uniform_buffer{};

	inline detail::create_buffer_fn_<buffer_type::shader_storage_buffer> create_shader_storage_buffer{};

	class vertex_array {
	public:

		vertex_array() noexcept;

		vertex_array(std::vector<vertex_attribute_format> format);

		vertex_array(const vertex_array& other);
		
		vertex_array(vertex_array&& other) noexcept;
		
		~vertex_array();
		
		auto operator= (const vertex_array& other) ->vertex_array&;
		
		auto operator= (vertex_array&& other) noexcept ->vertex_array&;
		
		auto swap(vertex_array& other) noexcept ->void;
		
		friend auto swap(vertex_array& lhs, vertex_array& rhs) noexcept ->void {
			lhs.swap(rhs);
		}
		
		auto bind() const ->void;

		auto unbind() const ->void;

		auto set_element_buffer(const element_buffer& ebo) const ->void;

		auto set_attribute_buffer(uint_t index, const vertex_buffer& vbo, uint_t offset = 0) const ->void;

		[[nodiscard]]
		auto format() const noexcept ->const std::vector<vertex_attribute_format>&;

		[[nodiscard]]
		auto get_id() const noexcept ->uint_t;
	private:
		uint_t id{};
		std::vector<vertex_attribute_format> format_;
	};
}