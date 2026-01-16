#pragma once

#include "thirdparty/signals.hpp"
#include "util/auto_release.hpp"
#include "util/config.hpp"
#include "util/convertible_to_pointer.hpp"
#include "util/flags.hpp"
#include "util/defer.hpp"
#include "util/method_macros.hpp"
#include "util/spans.hpp"

#include <future>
#include <math/vector.hpp>
#include <slcross.hpp>
#include <variant>

namespace stylizer {
	using namespace fteng;

	template<>
	struct auto_release<connection_raw> : public connection {
		using connection::connection;
		using connection::operator=;
	};

	auto_release(connection_raw) -> auto_release<connection_raw>;

	enum class error_severity {
		Verbose,
		Info,
		Warning,
		Error,
	};

	stylizer::signal<void(error_severity, std::string_view message, size_t error_tag)>& get_error_handler();
}
namespace stylizer::api {

	using bool32 = uint32_t;

	using vec2u = stdmath::vector<size_t, 2>;

	using vec3u = stdmath::vector<size_t, 3>;

	using color32 = stdmath::vector<stdmath::stl::float32_t, 4>;

	using color8 = stdmath::vector<uint8_t, 4>;

	using spirv = std::vector<uint32_t>;

	using spirv_view = std::span<uint32_t>;

	struct error : public std::runtime_error {
		using std::runtime_error::runtime_error;
		error(std::string_view message): std::runtime_error(std::string(message)) { }

		using severity = error_severity;
	};

	#ifndef STYLIZER_NO_EXCEPTIONS
		#define STYLIZER_API_THROW(x) (get_error_handler()(stylizer::error_severity::Error, (x), 0), throw error(x))
	#else
		#define STYLIZER_API_THROW(x) (get_error_handler()(stylizer::error_severity::Error, (x), 0), assert(false))
	#endif
	#include "texture_format.partial.h"

	enum class alpha_mode {
		Opaque,
		PostMultiplied,
		PreMultiplied,
		Inherit,
	};

	enum class usage {
		Invalid = 0,
		CopySource = (1 << 0),
		CopyDestination = (1 << 1),
		RenderAttachment = (1 << 2),
		TransientAttachment = (1 << 3),
		StorageAttachment = (1 << 4),
		Texture = (1 << 5),

		Storage = (1 << 8),
		Uniform = (1 << 9),
		Vertex = (1 << 6),
		Index = (1 << 7),
		MapRead = (1 << 10),
		MapWrite = (1 << 11),
	};

	enum class shader_language {
		SPIRV,
		GLSL,
		Slang,
		WGSL,
	};

	enum class shader_stage {
		Combined,
		Vertex,
		Fragment,
		Compute,
	};

	enum class comparison_function {
		NoDepthCompare,
		NeverPass,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,
		AlwaysPass,
	};

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
			Source,
			OneMinusSource,
			SourceAlpha,
			OneMinusSourceAlpha,
			Destination,
			OneMinusDestination,
			DestinationAlpha,
			OneMinusDestinationAlpha,
			SourceAlphaSaturated,
			Constant,
			OneMinusConstant,
		} source_factor = blend_state::factor::One, destination_factor = blend_state::factor::Zero;
	};
	struct color_attachment {
		STYLIZER_NULLABLE struct texture* texture = nullptr;

		STYLIZER_NULLABLE struct texture_view* view = nullptr;

		enum texture_format texture_format = texture_format::Undefined;

		STYLIZER_NULLABLE struct texture_view* resolve_target = nullptr;

		bool should_store = true;

		std::optional<color32> clear_value = {};

		std::optional<blend_state> color_blend_state = {};

		std::optional<blend_state> alpha_blend_state = {};
	};

	struct depth_stencil_attachment {
		struct stencil_config {
			bool should_store = true;

			std::optional<size_t> clear_value = {};

			bool readonly = false;

			struct face_state {
				enum class operation {
					Undefined,
					Keep,
					Zero,
					Replace,
					Invert,
					IncrementClamp,
					DecrementClamp,
					IncrementWrap,
					DecrementWrap
				};

				comparison_function compare = comparison_function::AlwaysPass;

				operation fail_operation = operation::Keep;
				operation depth_fail_operation = operation::Keep;
				operation pass_operation = operation::Keep;
			};

			face_state front = {};
			face_state back = {};

			uint32_t stencilReadMask = 0;
			uint32_t stencilWriteMask = 0;
		};

		STYLIZER_NULLABLE struct texture* texture = nullptr;

		STYLIZER_NULLABLE struct texture_view* view = nullptr;

		enum texture_format texture_format = texture_format::Undefined;

		bool should_store_depth = true;

		std::optional<float> depth_clear_value = {};

		bool depth_readonly = false;

		comparison_function depth_comparison_function = comparison_function::Less;

		size_t depth_bias = 0;

		float depth_bias_slope_scale = 0;

		float depth_bias_clamp = 0;

		std::optional<stencil_config> stencil = {};
	};

	struct temporary_return_t {};

	constexpr static temporary_return_t temporary_return;

	struct device;

	struct surface {
		struct texture_acquisition_failed : public error {
			using error::error;
		};

		enum class present_mode {
			Fifo = 0,
			FifoRelaxed,
			Immediate,
			Mailbox,
		};

		struct config {
			enum present_mode present_mode = surface::present_mode::Fifo;
			enum texture_format texture_format = texture_format::RGBAu8_NormalizedSRGB;
			enum alpha_mode alpha_mode = alpha_mode::Opaque;
			enum usage usage = usage::RenderAttachment;
			vec2u size;
		};

		virtual config determine_optimal_default_config(device& device, vec2u size) {
			config out {};
			out.size = size;
			return out;
		}

		virtual surface& configure(device& device, const config& config) = 0;

		virtual struct texture& next_texture(temporary_return_t, device& device) = 0;

		virtual texture_format configured_texture_format(device& device);

		virtual surface& present(device& device) = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~surface() = default;

		STYLIZER_API_AS_METHOD(surface);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(surface);
	};

	template<typename T>
	concept surface_concept = std::derived_from<T, surface> && requires(T t, void* display, void* surface, void* layer, void* instance, void* window, const std::string_view canvas_id, device device) {
		{ T::create_from_x11(display, window) } -> std::convertible_to<T>;
		{ T::create_from_wayland(display, surface) } -> std::convertible_to<T>;
		{ T::create_from_cocoa(layer, window) } -> std::convertible_to<T>;
		{ T::create_from_win32(window, instance) } -> std::convertible_to<T>;
		{ T::create_from_emscripten(canvas_id) } -> std::convertible_to<T>;
		{ t.next_texture(device) } -> std::derived_from<struct texture>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	struct texture_view {
		struct create_config {
			size_t base_mip_level = 0;
			std::optional<size_t> mip_level_count_override = {};
			bool treat_as_cubemap = false;


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

		virtual ~texture_view() = default;

		STYLIZER_API_AS_METHOD(texture_view);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(texture_view);
	};

	template<typename T>
	concept texture_view_concept = std::derived_from<T, texture_view> && requires(T t, device device, struct texture texture, texture_view::create_config config) {
		{ T::create(device, texture, config) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	struct texture {
		using format = api::texture_format;
		using view = texture_view;

		struct create_config {
			std::string_view label = "Stylizer Texture";
			enum texture_format format = format::RGBAu8_NormalizedSRGB;
			enum usage usage = usage::Texture;
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
			std::optional<address_mode> mode_x_override = {};
			std::optional<address_mode> mode_y_override = {};
			std::optional<address_mode> mode_z_override = {};
			bool magnify_linear = false;
			bool minify_linear = false;
			bool mip_linear = false;
			float lod_min_clamp = 0;
			float lod_max_clamp = 1;
			size_t max_anisotropy = 1;
			comparison_function depth_comparison_function = comparison_function::NoDepthCompare;
		};

		virtual texture_view& create_view(temporary_return_t, device& device, const view::create_config& config = {}) const = 0;

		virtual const texture_view& full_view(device& device, bool treat_as_cubemap = false) const = 0;

		virtual vec3u size() const = 0;

		virtual format texture_format() const = 0;

		virtual enum usage usage() const = 0;

		virtual uint32_t mip_levels() const = 0;

		virtual uint32_t samples() const = 0;

		virtual texture& configure_sampler(device& device, const sampler_config& config) = 0;

		virtual bool sampled() const = 0;

		virtual texture& write(device& device, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin = { { 0, 0, 0 } }, size_t mip_level = 0) = 0;

		virtual texture& copy_from(device& device, const texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = {}, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual texture& blit_from(device& device, const texture& source, std::optional<color32> clear_value = {}, STYLIZER_NULLABLE struct render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) = 0;

		virtual texture& generate_mipmaps(device& device, std::optional<size_t> first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~texture() = default;

		STYLIZER_API_AS_METHOD(texture);
		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(texture);
	};

	template<typename T>
	concept texture_concept = std::derived_from<T, texture> && requires(T t, device device, texture::create_config config, std::span<const std::byte> data, texture::data_layout layout) {
		{ T::create(device, config) } -> std::convertible_to<T>;
		{ T::create_and_write(device, data, layout, config) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	struct buffer {
		virtual const buffer& get_zero_buffer_singleton(device& device, enum usage usage = usage::Storage, size_t minimum_size = 0, STYLIZER_NULLABLE buffer* just_released = nullptr) = 0;

		virtual buffer& write(device& device, std::span<const std::byte> data, size_t offset = 0) = 0;

		virtual size_t size() const = 0;
		
		virtual enum usage usage() const = 0;

		virtual buffer& copy_from(device& device, const buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) = 0;

		virtual bool is_mapped() const = 0;

		virtual std::future<std::byte*> map_async(device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) = 0;

		virtual std::byte* map(device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) = 0;

		virtual void unmap() = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~buffer() = default;

		STYLIZER_API_AS_METHOD(buffer);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(buffer);
	};

	template<typename T>
	concept buffer_concept = std::derived_from<T, buffer> && requires(T t, device device, usage usage, size_t size, bool mapped_at_creation, std::span<const std::byte> data, size_t offset, const std::string_view label, buffer* just_released) {
		{ T::create(device, usage, size, mapped_at_creation, label) } -> std::convertible_to<T>;
		{ T::create_and_write(device, usage, data, offset, label) } -> std::convertible_to<T>;
		{ T::zero_buffer(device, usage, size, just_released) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	struct shader {
		using language = shader_language;
		using stage = shader_stage;

		static slcross::shader_stage to_slcross(stage stage) {
			switch (stage) {
			case shader_stage::Vertex: return slcross::shader_stage::vertex;
			case shader_stage::Fragment: return slcross::shader_stage::fragment;
			case shader_stage::Compute: return slcross::shader_stage::compute;
			default: STYLIZER_API_THROW("The combined stage is only valid for WGSL");
			}
		}

		template<typename T>
		inline static T create_from_source(device& device, language lang, stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") {
			auto entry_point = entry_point_.value_or("main");
			slcross::session session;
			defer_ { session.free(); };
			switch (lang) {
			break; case language::Slang: {
				auto module_name = std::string(label);
				module_name.erase(std::remove_if(module_name.begin(), module_name.end(), ::isspace), module_name.end());
				std::string path = module_name + ".slang";
				session = slcross::slang::parse_from_memory(source, path, module_name);
			}
			break; default: STYLIZER_API_THROW("SLCross doesn't support consuming the provided shader language!");
			}
			return T::create_from_session(device, stage, session, entry_point, label);
		}

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~shader() = default;

		STYLIZER_API_AS_METHOD(shader);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(shader);
	};

	template<typename T>
	concept shader_concept = requires(T t, device& device, shader::stage stage, slcross::session session, spirv_view spirv, const std::string_view entry_point, const std::string_view label) {
		{ T::create_from_session(device, stage, session, entry_point, label) } -> std::convertible_to<T>;
		{ T::create_from_spirv(device, stage, spirv, entry_point, label) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	struct pipeline {
		struct entry_point {
			STYLIZER_NULLABLE struct shader* shader = nullptr;
			const std::string_view entry_point_name = "main";
		};

		using entry_points = std::unordered_map<shader::stage, entry_point>;
	};

	struct command_buffer {
		stylizer::signal<void()> deferred_to_release;

		template<std::convertible_to<std::function<void>> Tfunc>
		command_buffer& defer(Tfunc&& func) {
			deferred_to_release.connect(std::move(func));
			return *this;
		}

		virtual void submit(device& device, bool release = true) = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~command_buffer() = default;

		STYLIZER_API_AS_METHOD(command_buffer);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(command_buffer);
	};

	struct bind_group {
		struct buffer_binding {
			const struct buffer* buffer;
			size_t offset = 0;
			std::optional<size_t> size_override = {};
		};

		struct texture_binding {
			STYLIZER_NULLABLE const struct texture* texture = nullptr;
			STYLIZER_NULLABLE const struct texture_view* texture_view = nullptr;
			std::optional<bool> sampled_override = {};
		};

		using binding = std::variant<buffer_binding, texture_binding>;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~bind_group() = default;

		STYLIZER_API_AS_METHOD(bind_group);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(bind_group);
	};

	struct compute_pipeline : public pipeline {
		using pass = struct command_encoder;

		virtual bind_group& create_bind_group(temporary_return_t, device& device, size_t index, std::span<const bind_group::binding> bindings) = 0;

		static void quick_dispatch(device& device, vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings = {});

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~compute_pipeline() = default;

		STYLIZER_API_AS_METHOD(compute_pipeline);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(compute_pipeline);
	};

	template<typename T>
	concept compute_pipeline_concept = std::derived_from<T, compute_pipeline> && requires(T t, device device, pipeline::entry_point entry_point, const std::string_view label) {
		{ T::create(device, entry_point, label) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	template<typename Treturn>
	struct command_encoder_base {
		using pipeline = compute_pipeline;

		virtual Treturn& defer(std::function<void()>&& func) = 0;

		virtual Treturn& copy_buffer_to_buffer(device& device, buffer& destination, const buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) = 0;

		virtual Treturn& copy_buffer_to_texture(device& device, buffer& destination, const texture& source, std::optional<size_t> destination_offset = 0, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual Treturn& copy_texture_to_buffer(device& device, texture& destination, const buffer& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<size_t> source_offset = 0, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual Treturn& copy_texture_to_texture(device& device, texture& destination, const texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		virtual Treturn& bind_compute_pipeline(device& device, const compute_pipeline& pipeline, bool release_on_submit = false) = 0;

		virtual Treturn& bind_compute_group(device& device, const bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) = 0;

		virtual Treturn& dispatch_workgroups(device& device, vec3u workgroups) = 0;

		virtual command_buffer& end(temporary_return_t, device& device) = 0;

		virtual void one_shot_submit(device& device) = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~command_encoder_base() = default;

		STYLIZER_API_AS_METHOD(command_encoder_base);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(Treturn);
	};

	struct command_encoder : public command_encoder_base<command_encoder> {};

	template<typename T>
	concept command_encoder_concept = std::derived_from<T, command_encoder> && requires(T t, device device, bool one_shot, const std::string_view label) {
		{ t.deferred_to_release } -> convertible_to_pointer<stylizer::signal<void()>>;
		{ T::create(device, one_shot, label) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	namespace compute {
		using pass = command_encoder;
		using pipeline = compute_pipeline;
	}

	struct render_pass : public command_encoder_base<render_pass> {
		using blend_state = api::blend_state;
		using color_attachment = api::color_attachment;
		using depth_stencil_attachment = api::depth_stencil_attachment;
		using pipeline = struct render_pipeline;

		virtual render_pass& bind_render_pipeline(device& device, const render_pipeline& pipeline, bool release_on_submit = false) = 0;

		virtual render_pass& bind_render_group(device& device, const bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) = 0;

		virtual render_pass& bind_vertex_buffer(device& device, size_t slot, const buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) = 0;

		virtual render_pass& bind_index_buffer(device& device, const buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) = 0;

		virtual render_pass& draw(device& device, size_t vertex_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_vertex = 0, size_t first_instance = 0) = 0;

		virtual render_pass& draw_indexed(device& device, size_t index_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_index = 0, std::optional<size_t> base_vertex = 0, size_t first_instance = 0) = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		STYLIZER_API_AS_METHOD(render_pass);
	};

	template<typename T>
	concept render_pass_concept = std::derived_from<T, render_pass> && requires(T t, device device, std::span<const render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth, bool one_shot, const std::string_view label) {
		{ T::create(device, colors, depth, one_shot, label) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	template<typename T>
	struct vertex_buffer_type_format;

	struct render_pipeline : public pipeline {
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
				TriangleStrip
			} primitive_topology = primitive_topology::TriangleList;

			float line_pixel_width = 1;

			enum class cull_mode {
				Back,
				Front,
				Both,
				Neither
			} cull_mode = cull_mode::Neither;

			bool winding_order_clockwise = false;

			bool u16_indices = false;

			struct vertex_buffer_layout {
				struct attribute {
					enum class format {
						Invalid,
						f32x1,
						f32x2,
						f32x3, // Note: GPUs expect them to be aligned on 4
						f32x4,
						f16x2,
						f16x4,
						i32x1,
						i32x2,
						i32x3, // Note: GPUs expect them to be aligned on 4
						i32x4,
						i16x2,
						i16x4,
						i16x2_normalized,
						i16x4_normalized,
						i8x2,
						i8x4,
						i8x2_normalized,
						i8x4_normalized,
						u32x1,
						u32x2,
						u32x3, // Note: GPUs expect them to be aligned on 4
						u32x4,
						u16x2,
						u16x4,
						u16x2_normalized,
						u16x4_normalized,
						u8x2,
						u8x4,
						u8x2_normalized,
						u8x4_normalized,
						u10_10_10_2_normalized,
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
				std::optional<size_t> stride_override = {};
			};
			std::vector<vertex_buffer_layout> vertex_buffers = {};
			using attribute_format = STYLIZER_ENUM_CLASS vertex_buffer_layout::attribute::format;

			struct sampling {
				uint32_t count = 1;
				uint32_t mask = ~0u;
				bool alpha_to_coverage = false;
			} multisampling = {};
		};

		virtual bind_group& create_bind_group(temporary_return_t, device& device, size_t index, std::span<const bind_group::binding> bindings) = 0;

		virtual operator bool() const { return false; }

		virtual void release() = 0;

		virtual ~render_pipeline() = default;

		STYLIZER_API_AS_METHOD(render_pipeline);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(render_pipeline);
	};

	template<typename T>
	concept render_pipeline_concept = std::derived_from<T, render_pipeline> && requires(T t, device device, pipeline::entry_points entry_points, render_pipeline::config config, const std::string_view label, std::span<const color_attachment> color_attachments, std::optional<depth_stencil_attachment> depth_attachment, render_pass compatible_render_pass) {
		{ T::create(device, entry_points, color_attachments, depth_attachment, config, label) } -> std::convertible_to<T>;
		{ T::create_from_compatible_render_pass(device, entry_points, compatible_render_pass, config, label) } -> std::convertible_to<T>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	namespace render {
		using pass = render_pass;
		using pipeline = render_pipeline;
	}

	struct device {
		struct create_config {
			const std::string_view label = "Stylizer Device";
			const std::string_view queue_label = "Stylizer Queue";
			bool high_performance = true;
			STYLIZER_NULLABLE struct surface* compatible_surface = nullptr;
		};

		virtual bool tick(bool wait_for_queues = true) = 0;

		virtual texture& create_texture(temporary_return_t, const texture::create_config& config = {}) = 0;

		virtual texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const texture::data_layout& layout, const texture::create_config& config = {}) = 0;

		virtual buffer& create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") = 0;

		virtual buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") = 0;

		virtual const buffer& get_zero_buffer_singleton(usage usage = usage::Storage, size_t minimum_size = 0, STYLIZER_NULLABLE buffer* just_released = nullptr) {
			auto& tmp = create_buffer(temporary_return, usage::Storage, 0);
			auto& out = tmp.get_zero_buffer_singleton(*this, usage, minimum_size, just_released);
			tmp.release();
			return out;
		}

		virtual shader& create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") = 0;

		virtual shader& create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") = 0;

		virtual shader& create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") = 0;

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

		virtual ~device() = default;

		STYLIZER_API_AS_METHOD(device);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(device);
	};

	template<typename T>
	concept device_concept = std::derived_from<T, device> && requires(T t, device::create_config config, texture::create_config texture_config, std::span<const render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth, bool one_shot, const std::string_view label, usage usage, size_t size, bool mapped_at_creation, std::span<const std::byte> data, size_t offset) {
		{ T::create_default(config) } -> std::convertible_to<T>;
		{ t.create_texture(texture_config) } -> std::derived_from<texture>;
		{ t.create_buffer(usage, size, mapped_at_creation, label) } -> std::derived_from<buffer>;
		{ t.create_and_write_buffer(usage, data, offset, label) } -> std::derived_from<buffer>;
		{ t.create_render_pass(colors, depth, one_shot, label) } -> std::derived_from<render_pass>;
		{ t.auto_release() } -> std::convertible_to<auto_release<T>>;
		{ t.type } -> std::convertible_to<size_t>;
	};

	inline texture::format surface::configured_texture_format(device& device) {
		auto texture = std::unique_ptr<struct texture>(next_texture(temporary_return, device).move_temporary_to_heap());
		auto out = texture->texture_format();
		present(device);
		return out;
	}
	inline void compute_pipeline::quick_dispatch(device& device, vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings) {
		auto& pipeline = device.create_compute_pipeline(temporary_return, entry_point);
		device.create_command_encoder(temporary_return, true)
			.bind_compute_pipeline(device, pipeline)
			.bind_compute_group(device, pipeline.create_bind_group(temporary_return, device, 0, bindings), true)
			.dispatch_workgroups(device, workgroups)
			.one_shot_submit(device);
		pipeline.release();
	}
	template<typename T>
	struct vertex_buffer_type_format {
		static constexpr auto format = render_pipeline::config::vertex_buffer_layout::attribute::format::Invalid;
	};
	template<>
	struct vertex_buffer_type_format<float> {
		static constexpr auto format = render_pipeline::config::vertex_buffer_layout::attribute::format::f32x1;
	};
}