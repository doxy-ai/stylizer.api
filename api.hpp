#pragma once

#include <future>
#include <optional>
#include <slcross.hpp>

#include <variant>
#include <concepts>
#include <stdexcept>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>

#include "spans.hpp"

namespace stylizer {
	template<typename T>
	concept releasable = requires(T t) {
		{t.release()};
	};

	template<typename T>
	struct auto_release: public T {
		auto_release() : T() {}
		auto_release(T&& raw) : T(std::move(raw)) {}
		auto_release(const auto_release&) = delete;
		auto_release(auto_release&& other) : T(std::move(other)) {}
		auto_release& operator=(T&& raw) { static_cast<T&>(*this) = std::move(raw); return *this; }
		auto_release& operator=(const auto_release&) = delete;
		auto_release& operator=(auto_release&& other) {{ static_cast<T&>(*this) = std::move(static_cast<T&>(other)); return *this; }}

		~auto_release() { if(*this) this->release(); }
	};

#define STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(type)\
		type() {}\
		type(const type&) = default;\
		type& operator=(const type&) = default;

#define STYLIZER_API_AS_METHOD(type) template<std::derived_from<type> T>\
	T& as() {\
		T* out = dynamic_cast<T*>(this);\
		assert(out != nullptr);\
		return *out;\
	}
}

namespace stylizer::api {
	inline namespace operators {
		using namespace magic_enum::bitwise_operators;
	}

#ifdef __cpp_exceptions
	struct error: public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	#define STYLIZER_API_THROW(x) throw error(x)
#else
	#define STYLIZER_API_THROW(x) assert((x, false))
#endif

	enum class alpha_mode {
		Opaque,
		PostMultiplied,
		PreMultiplied,
		Inherit,
	};

	enum class texture_format {
		Undefined,
		Depth24,

		Rf32,
		Ru8,

		RGBA32,

		RGBA8,
		RGBA8_SRGB,

		BGRA8_SRGB,

		Gray32 = Rf32,
		Gray8 = Ru8,
	};

	inline size_t bytes_per_pixel(texture_format format) {
		switch(format){
			case texture_format::Depth24: return 24 / 8;
			case texture_format::Rf32: return sizeof(float);
			case texture_format::Ru8: return sizeof(uint8_t);
			case texture_format::RGBA32: return sizeof(color32);
			case texture_format::RGBA8: return sizeof(color8);
			case texture_format::RGBA8_SRGB: return sizeof(color8);
			case texture_format::BGRA8_SRGB: return sizeof(color8);
		}
		STYLIZER_API_THROW("An error has occurred finding the number of bytes per pixel in format: " + std::string(magic_enum::enum_name(format)));
	}

	enum class usage {
		Invalid = 0,
		CopySource = (1 << 0),
		CopyDestination = (1 << 1),
		RenderAttachment = (1 << 2),
		TextureBinding = (1 << 3),

		Vertex = (1 << 4),
		Index = (1 << 5),
		Storage = (1 << 6),
		Uniform = (1 << 7),
		MapRead = (1 << 8),
		MapWrite = (1 << 9),
	};

	enum class shader_language {
		SPIRV,
		GLSL,
		Slang,
		WGSL,
	};

	enum class shader_stage {
		Combined, // Only valid for WGSL
		Vertex,
		Fragment
	};

	enum class comparison_function {
		NoDepthCompare,
		Never,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,
		Always,
	};


	using bool32 = uint32_t;
	struct vec2u { size_t x = 0, y = 0; };
	struct vec3u { size_t x = 0, y = 0, z = 0; };
	struct color32 { float r = 0, g = 0, b = 0, a = 1; };
	struct color8 { uint8_t r = 0, g = 0, b = 0, a = 1; };



	struct blend_state {
		enum class operation {
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max,
		} operation = blend_state::operation::Add;
		enum class factor {
			Zero,
			One,
			Src,
			OneMinusSrc,
			SrcAlpha,
			OneMinusSrcAlpha,
			Dst,
			OneMinusDst,
			DstAlpha,
			OneMinusDstAlpha,
			SrcAlphaSaturated,
			Constant,
			OneMinusConstant,
		} source_factor = blend_state::factor::One, destination_factor = blend_state::factor::Zero;
	};
	struct color_attachment {
		struct texture* texture = nullptr; struct texture_view* view = nullptr; enum texture_format texture_format = texture_format::Undefined; // Only one of these need be specified, they will be calculated as needed (note if creating a pass only specifying the format will produce an error!)
		struct texture_view* resolve_target = nullptr; // Used for multisampling
		bool should_store = true; // False discards
		std::optional<color32> clear_value = {}; // When set value is not loaded
		std::optional<blend_state> color_blend_state = {};
		std::optional<blend_state> alpha_blend_state = {};
	};
	struct depth_stencil_attachment {
		struct texture* texture = nullptr; struct texture_view* view = nullptr; enum texture_format texture_format = texture_format::Undefined; // Only one of these need be specified, they will be calculated as needed (note if creating a pass only specifying the format will produce an error!)
		bool should_store_depth = true; // False discards
		std::optional<float> depth_clear_value = {}; // When set value is not loaded
		bool depth_readonly = false;
		comparison_function depth_comparison_function = comparison_function::Less;
		size_t depth_bias = 0;
		float depth_bias_slope_scale = 0;
		float depth_bias_clamp = 0;

		bool should_store_stencil = false; // False discards
		std::optional<size_t> stencil_clear_value = {}; // When set value is not loaded // TODO: Should be uint32_t?
		bool stencil_readonly = false;
		// WGPUStencilFaceState stencilFront;
		// WGPUStencilFaceState stencilBack;
		// uint32_t stencilReadMask;
		// uint32_t stencilWriteMask;
	};



	struct temporary_return_t{};
	constexpr static temporary_return_t temporary_return;

	struct device;

	struct surface {
		struct texture_acquisition_failed : public error { using error::error; };

		enum class present_mode {
			Fifo = 0,
			FifoRelaxed,
			Immediate,
			Mailbox,
		};

		struct config {
			enum present_mode present_mode = surface::present_mode::Fifo;
			enum texture_format texture_format = texture_format::RGBA8_SRGB;
			enum alpha_mode alpha_mode = alpha_mode::Opaque;
			enum usage usage = usage::RenderAttachment;
			vec2u size;
		};
		virtual config determine_optimal_config(device& device, vec2u size) { config out{}; out.size = size; return out; }
		virtual surface& configure(device& device, const config& config) = 0;

		virtual struct texture& next_texture(temporary_return_t, device& device) = 0; // May throw texture_acquisition_failed
		virtual surface& present(device& device) = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(surface);
	};

	template<typename T>
	concept surface_concept = std::derived_from<T, surface> && requires(T t, void* display, void* surface, void* layer, void* instance, void* window, const std::string_view canvas_id, device device) {
		{ T::create_from_x11(display, window) } -> std::convertible_to<T>;
		{ T::create_from_wayland(display, surface) } -> std::convertible_to<T>;
		{ T::create_from_cocoa(layer, window) } -> std::convertible_to<T>;
		{ T::create_from_win32(window, instance) } -> std::convertible_to<T>;
		{ T::create_from_emscripten(canvas_id) } -> std::convertible_to<T>;

		{ t.next_texture(device) } -> std::derived_from<struct texture>;
	};

	struct texture_view {
		struct create_config {
			size_t base_mip_level = 0;
			std::optional<size_t> mip_level_count_override = {};
			// .baseArrayLayer = 0,
			// .arrayLayerCount = std::max<uint32_t>(texture_.getDepthOrArrayLayers(), 1),
			// bool view_depth = true; // TODO: Do we need this or is just computing it from the format good enough?

			enum class aspect {
				All = 0,
				DepthOnly,
				StencilOnly
			} aspect = aspect::All;
		};

		virtual const struct texture& texture() const = 0;
		operator const struct texture&() const { return texture(); }

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(texture_view);
	};

	template<typename T>
	concept texture_view_concept = std::derived_from<T, texture_view> && requires(T t, device device, struct texture texture, texture_view::create_config config) {
		{ T::create(device, texture, config) } -> std::convertible_to<T>;
	};

	struct texture {
		using format = api::texture_format;
		using view = texture_view;

		struct create_config {
			const std::string_view label = "Stylizer Texture";
			enum texture_format format = format::RGBA8_SRGB;
			enum usage usage = usage::TextureBinding;
			vec3u size;
			uint32_t mip_levels = 1;
			uint32_t samples = 1;
			bool cubemap = false;
		};

		struct data_layout {
			size_t offset;
			size_t bytes_per_row;
			size_t rows_per_image;
		};

		enum class address_mode {
			Repeat,
			MirrorRepeat,
			ClampToEdge,
		};

		struct sampler_config {
			const std::string_view label = "Stylizer Sampler";
			address_mode mode = address_mode::Repeat;
			bool magnify_linear = false;
			bool minify_linear = false;
			bool mip_linear = false;
			float lod_min_clamp = 0;
			float lod_max_clamp = 1;
			size_t max_anisotropy = 1;
			comparison_function depth_comparison_function = comparison_function::NoDepthCompare;
		};

		virtual texture_view& create_view(temporary_return_t, device& device, const view::create_config& config = {}) const = 0;
		virtual const texture_view& full_view(device& device) const = 0;

		virtual vec3u size() const = 0;
		virtual format texture_format() const = 0;
		virtual enum usage usage() const = 0;
		virtual uint32_t mip_levels() const = 0;
		virtual uint32_t samples() const = 0;

		virtual texture& configure_sampler(device& device, const sampler_config& config) = 0;
		virtual bool sampled() const = 0;
		virtual texture& write(device& device, std::span<const std::byte> data, const data_layout& layout, vec3u extent, vec3u origin = {0, 0, 0}, size_t mip_level = 0) = 0;

		virtual texture& copy_from(device& device, const texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;
		virtual texture& blit_from(device& device, const texture& source, std::optional<color32> clear_value = {}, struct render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) = 0;

		virtual texture& generate_mipmaps(device& device, size_t first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(texture);
	};

	template<typename T>
	concept texture_concept = std::derived_from<T, texture> && requires(T t, device device, texture::create_config config, std::span<const std::byte> data, texture::data_layout layout) {
		{ T::create(device, config) } -> std::convertible_to<T>;
		{ T::create_and_write(device, data, layout, config) } -> std::convertible_to<T>;
	};

	struct buffer {
		virtual const buffer& get_zero_buffer_singleton(device& device, usage usage = usage::Storage, size_t minimum_size = 0, buffer* just_released = nullptr) = 0;

		virtual buffer& write(device& device, std::span<const std::byte> data, size_t offset = 0) = 0;
		virtual size_t size() const = 0;

		virtual buffer& copy_from(device& device, const buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) = 0;

		virtual bool is_mapped() const = 0;
		virtual std::future<std::byte*> map_async(device& device, bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) = 0;
		virtual std::byte* map(device& device, bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) = 0;
		virtual void unmap() = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(buffer);
	};

	template<typename T>
	concept buffer_concept = std::derived_from<T, buffer> && requires(T t, device device, usage usage, size_t size, bool mapped_at_creation, std::span<const std::byte> data, size_t offset, const std::string_view label, buffer* just_released) {
		{ T::create(device, usage, size, mapped_at_creation, label) } -> std::convertible_to<T>;
		{ T::create_and_write(device, usage, data, offset, label) } -> std::convertible_to<T>;

		{ T::zero_buffer(device, usage, size, just_released) } -> std::convertible_to<T>;
	};

	struct shader {
		using language = shader_language;
		using stage = shader_stage;

		static slcross::shader_stage to_slcross(stage stage) {
			switch(stage){
			case shader_stage::Vertex: return slcross::shader_stage::Vertex;
			case shader_stage::Fragment: return slcross::shader_stage::Fragment;
			default: throw std::invalid_argument("The combined stage is only valid for WGSL");
			}
		}

		template<typename T>
		inline static T create_from_source(device& device, language lang, const std::string_view source, stage stage = shader_stage::Combined, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") {
			slcross::spirv module;
			switch(lang) {
			break; case language::GLSL:
				module = slcross::glsl::parse_from_memory(to_slcross(stage), source, entry_point);
			break; case language::Slang: {
				static slcross::slang::session* session = nullptr;
				if(!session) session = slcross::slang::create_session();

				std::string module_name = slcross::detail::remove_whitespace(std::string{label});
				std::string path = module_name + ".slang";
				module = slcross::slang::parse_from_memory(session, source, entry_point, path, module_name); // TODO: Multiple shaders with same module cause issues?
			}
			break; case language::WGSL:
				module = slcross::wgsl::parse_from_memory(source);
			break; default: throw std::invalid_argument("SPIRV is not a valid language to compile from source");
			}
			return T::create_from_spirv(device, module, label);
		}

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(shader);
	};

	template<typename T>
	concept shader_concept = requires(T t, device& device, slcross::spirv_view spirv, const std::string_view label) {
		{ T::create_from_spirv(device, spirv, label) } -> std::convertible_to<T>;
	};




	struct pipeline {
		struct entry_point {
			struct shader* shader = nullptr;
			const std::string_view entry_point_name = "main";
		};
		using entry_points = std::unordered_map<shader::stage, entry_point>;
	};

	struct command_buffer {
		virtual void submit(device& device, bool release = true) = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(command_buffer);
	};

	struct bind_group {
		struct buffer_binding {
			const struct buffer* buffer;
			size_t offset = 0;
			std::optional<size_t> size_override = {};
		};
		struct texture_binding {
			const struct texture* texture = nullptr; const struct texture_view* texture_view = nullptr; // Only one needs to be specified... if texture is provided the view is extracted from it
			std::optional<bool> sampled_override = {};
		};
		using binding = std::variant<buffer_binding, texture_binding>;
		// TODO: Need create functions?

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(bind_group);
	};

	struct compute_pipeline: public pipeline {
		using pass = struct command_buffer;

		virtual bind_group& create_bind_group(temporary_return_t, device& device, size_t index, std::span<const bind_group::binding> bindings) = 0;

		static void quick_dispatch(device& device, vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings = {});

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(compute_pipeline);
	};

	template<typename T>
	concept compute_pipeline_concept = std::derived_from<T, compute_pipeline> && requires(T t, device device, pipeline::entry_point entry_point, const std::string_view label) {
		{ T::create(device, entry_point, label) } -> std::convertible_to<T>;
	};

	struct command_encoder {
		using pipeline = compute_pipeline;

		// TODO: Copy functions
		virtual command_encoder& copy_buffer_to_buffer(device& device, buffer& destination, const buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) = 0;
		virtual command_encoder& copy_texture_to_texture(device& device, texture& destionation, const texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual command_encoder& bind_compute_pipeline(device& device, const compute_pipeline& pipeline) = 0;
		virtual command_encoder& bind_compute_group(device& device, const bind_group& group, std::optional<size_t> index_override = {}) = 0;
		virtual command_encoder& dispatch_workgroups(device& device, vec3u workgroups) = 0;

		virtual command_buffer& end(temporary_return_t, device& device) = 0;
		virtual void one_shot_submit(device& device) = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(command_encoder);
	};

	template<typename T>
	concept command_encoder_concept = std::derived_from<T, command_encoder> && requires(T t, device device, bool one_shot, const std::string_view label) {
		{ T::create(device, one_shot, label) } -> std::convertible_to<T>;
	};

	namespace compute {
		using pass = command_encoder;
		using pipeline = compute_pipeline;
	}



	struct render_pass: public command_encoder {
		using blend_state = api::blend_state;
		using color_attachment = api::color_attachment;
		using depth_stencil_attachment = api::depth_stencil_attachment;
		using pipeline = struct render_pipeline;

		virtual render_pass& bind_render_pipeline(device& device, const render_pipeline& pipeline) = 0;
		virtual render_pass& bind_render_group(device& device, const bind_group& group, std::optional<size_t> index_override = {}) = 0;
		virtual render_pass& bind_vertex_buffer(device& device, size_t slot, const buffer& buffer_, size_t offset = 0, std::optional<size_t> size_override = {}) = 0;
		virtual render_pass& bind_index_buffer(device& device, const buffer& buffer_, size_t offset = 0, std::optional<size_t> size_override = {}) = 0;
		virtual render_pass& draw(device& device, size_t vertex_count, size_t instance_count = 1, size_t first_vertex = 0, size_t first_instance = 0) = 0;
		virtual render_pass& draw_indexed(device& device, size_t index_count, size_t instance_count = 1, size_t first_index = 0, size_t base_vertex = 0, size_t first_instance = 0) = 0;

		virtual command_buffer& end(temporary_return_t, device& device) = 0;
		virtual void one_shot_submit(device& device) = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(render_pass);
	};

	template<typename T>
	concept render_pass_concept = std::derived_from<T, render_pass> && requires(T t, device device, std::span<const render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth, bool one_shot, const std::string_view label) {
		{ T::create(device, colors, depth, /*is the resulting command buffer intended to only be used once?*/one_shot, label) } -> std::convertible_to<T>;
	};

	template<typename T>
	struct vertex_buffer_type_format;

	struct render_pipeline: public pipeline {
		using blend_state = api::blend_state;
		using color_attachment = api::color_attachment;
		using depth_stencil_attachment = api::depth_stencil_attachment;
		using pass = render_pass;

		struct config {
			enum class primitive_topology {
				PointList,
				LineList,
				LineStrip,
				TriangleList,
				TriangleStrip,
			} primitive_topology = primitive_topology::TriangleList;

			float line_pixel_width = 1;

			enum class cull_mode {
				Back,
				Front,
				Both,
				Neither
			} cull_mode = cull_mode::Neither;
			bool winding_order_clockwise = true;
			bool u16_indices = false; // False implies u32 indices

			struct vertex_buffer_layout {
				struct attribute {
					enum class format {
						Invalid,
						f32x1,
						f32x2,
						f32x3,
						f32x4,
						i32x1,
						i32x2,
						i32x3,
						i32x4,
						u32x1,
						u32x2,
						u32x3,
						u32x4,
					} format = format::f32x4;
					std::optional<size_t> offset_override = {};
					std::optional<size_t> shader_location_override = {};

					template<typename T>
					static constexpr enum format format_of(const T& ref = {}) {
						return vertex_buffer_type_format<T>::format;
					}
				};
				std::vector<attribute> attributes;
				bool per_instance = false;
				std::optional<size_t> stride_override = {}; // If not set stride is calculated from attributes
			};
			std::vector<vertex_buffer_layout> vertex_buffers = {};

			// TODO: Multisampling
			// TODO: Depth/stencil
		};

		virtual bind_group& create_bind_group(temporary_return_t, device& device, size_t index, std::span<const bind_group::binding> bindings) = 0;

		virtual operator bool() const { return false; }
		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(render_pipeline);
	};

	template<typename T>
	concept render_pipeline_concept = std::derived_from<T, render_pipeline> && requires(T t,
		device device, pipeline::entry_points entry_points, render_pipeline::config config, const std::string_view label,
		std::span<const color_attachment> color_attachments, std::optional<depth_stencil_attachment> depth_attachment,
		render_pass compatible_render_pass
	) {
		{ T::create(device, entry_points, color_attachments, depth_attachment, config, label) } -> std::convertible_to<T>;
		{ T::create_from_compatible_render_pass(device, entry_points, compatible_render_pass, config, label) } -> std::convertible_to<T>;
	};

	namespace render {
		using pass = render_pass;
		using pipeline = render_pipeline;
	}



	struct device {
		struct create_config {
			const std::string_view label = "Stylizer Device";
			bool high_performance = true;
			struct surface* compatible_surface = nullptr; // When set to null the device will be configured in headless mode
		};

		virtual bool wait(bool for_queues = true) = 0; // Wait for device to finish

		virtual texture& create_texture(temporary_return_t, const texture::create_config& config = {}) = 0;
		virtual texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const texture::data_layout& layout, const texture::create_config& config = {}) = 0;

		virtual buffer& create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") = 0;
		virtual buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") = 0;

		// NOTE: This function is more costly than the one on buffer (since it needs to create a temporary buffer), and thus should be avoided when possible!
		virtual const buffer& get_zero_buffer_singleton(usage usage = usage::Storage, size_t minimum_size = 0, buffer* just_released = nullptr) {
			auto& tmp = create_buffer(temporary_return, usage::Storage, 0);
			auto& out = tmp.get_zero_buffer_singleton(*this, usage, minimum_size, just_released);
			tmp.release();
			return out;
		}

		virtual shader& create_shader_from_spirv(temporary_return_t, slcross::spirv_view spirv, const std::string_view label = "Stylizer Shader") = 0;

		virtual command_encoder& create_command_encoder(temporary_return_t, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") = 0;
		virtual render_pass& create_render_pass(temporary_return_t, std::span<const render_pass::color_attachment> colors, const std::optional<render_pass::depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") = 0;

		virtual compute_pipeline& create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") = 0;
		virtual render_pipeline& create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") = 0;
		virtual render_pipeline& create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") = 0;

		device& quick_compute_dispatch(vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings = {}) {
			compute_pipeline::quick_dispatch(*this, workgroups, entry_point, bindings);
			return *this;
		}

		virtual operator bool() const { return false; }
		virtual void release(bool static_sub_objects = false) = 0;
		STYLIZER_API_AS_METHOD(device);
	};

	template<typename T>
	concept device_concept = std::derived_from<T, device> && requires(T t, device::create_config config,
		texture::create_config texture_config,
		std::span<const render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth, bool one_shot, const std::string_view label,
		usage usage, size_t size, bool mapped_at_creation, std::span<const std::byte> data, size_t offset
	) {
		{ T::create_default(config) } -> std::convertible_to<T>;

		{ t.create_texture(texture_config) } -> std::derived_from<texture>;

		{ t.create_buffer(usage, size, mapped_at_creation, label) } -> std::derived_from<buffer>;
		{ t.create_and_write_buffer(usage, data, offset, label) } -> std::derived_from<buffer>;

		{ t.create_render_pass(colors, depth, one_shot, label) } -> std::derived_from<render_pass>;
	};

	inline void compute_pipeline::quick_dispatch(device& device, vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings /* = {} */) {
		auto& pipeline = device.create_compute_pipeline(temporary_return, entry_point);
		device.create_command_encoder(temporary_return, true)
			.bind_compute_pipeline(device, pipeline)
			.bind_compute_group(device, pipeline.create_bind_group(temporary_return, device, 0, bindings))
			.dispatch_workgroups(device, workgroups)
			.one_shot_submit(device);
		pipeline.release();
	}

	template<typename T>
	struct vertex_buffer_type_format { static constexpr auto format = render_pipeline::config::vertex_buffer_layout::attribute::format::Invalid; };
	template<>
	struct vertex_buffer_type_format<float> { static constexpr auto format = render_pipeline::config::vertex_buffer_layout::attribute::format::f32x1; };

}