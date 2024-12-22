#pragma once

#include <concepts>
#include <span>
#include <stdexcept>
#include <magic_enum/magic_enum.hpp>

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

	template<typename T>
	std::span<std::byte> byte_span(std::span<T> span) { return {(std::byte*)span.data(), span.size_bytes()}; }

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
	using namespace magic_enum::bitwise_operators;

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
		RenderAttachment = (1 << 0),
		TextureBinding = (1 << 1),
	};

	enum class comparison_function {
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

	struct temporary_return_t{};
	constexpr static temporary_return_t temporary_return;

	struct device;

	struct surface {
		struct texture_aquisition_failed : public error { using error::error; };

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

		virtual struct texture& next_texture(temporary_return_t, device& device) = 0; // May throw texture_aquisition_failed
		virtual surface& present(device& device) = 0;

		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(surface);
	};

	template<typename T>
	concept surface_concept = std::derived_from<T, surface> && requires(T t, void* display, void* surface, void* layer, void* instance, void* window, std::string_view canvas_id, device device) {
		{ T::create_from_x11(display, window) } -> std::convertible_to<T>;
		{ T::create_from_wayland(display, surface) } -> std::convertible_to<T>;
		{ T::create_from_cocoa(layer, window) } -> std::convertible_to<T>;
		{ T::create_from_win32(window, instance) } -> std::convertible_to<T>;
		{ T::create_from_emscripten(canvas_id) } -> std::convertible_to<T>;

		{ t.next_texture(device) } -> std::derived_from<struct texture>;
	};

	struct texture {
		using format = texture_format;
		
		struct create_config {
			std::string_view label = "Stylizer Texture";
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
			std::string_view label = "Stylizer Sampler";
			address_mode mode = address_mode::Repeat;
			bool magnify_linear = false;
			bool minify_linear = false;
			bool mip_linear = false;
			float lod_min_clamp = 0;
			float lod_max_clamp = 1;
			size_t max_anisotropy = 0;
			comparison_function depth_comparison_function = comparison_function::Less;
		};

		virtual texture& configure_sampler(device& device, const sampler_config& config) = 0;
		virtual bool sampled() const = 0;
		virtual texture& write(device& device, std::span<std::byte> data, data_layout layout, vec3u extent, vec3u origin = {0, 0, 0}, size_t mip_level = 0) = 0;

		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(texture);
	};

	template<typename T>
	concept texture_concept = std::derived_from<T, texture> && requires(T t, device device, texture::create_config config, std::span<const std::byte> data, texture::data_layout layout) {
		{ T::create(device, config) } -> std::convertible_to<T>;
		{ T::create_and_write(device, data, layout, config) } -> std::convertible_to<T>;
	};




	struct command_buffer {
		virtual void submit(device& device, bool release = true) = 0;

		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(command_buffer);
	};

	struct command_encoder {
		// TODO: Add copy methods

		// virtual void release() = 0;
		// STYLIZER_API_AS_METHOD(command_encoder);
	};

	struct render_pass: public command_encoder {
		struct color_attachment {
			struct texture& texture;
			struct texture* resolve_target = nullptr; // Used for multisampling
			bool should_store = true; // False discards
			std::optional<color32> clear_value = {}; // When set value is not loaded
		};
		struct depth_stencil_attachment {
			struct texture& texture;
			bool should_store_depth = true; // False discards
			std::optional<float> depth_clear_value = {}; // When set value is not loaded
			bool depth_readonly = false;

			bool should_store_stencil = true; // False discards
			std::optional<size_t> stencil_clear_value = {}; // When set value is not loaded // TODO: Should be uint32_t?
			bool stencil_readonly = false;
		};

		virtual command_buffer& end(temporary_return_t, device& device) = 0;
		virtual void submit(device& device, bool release = true) = 0;

		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(render_pass);
	};

	template<typename T>
	concept render_pass_concept = std::derived_from<T, render_pass> && requires(T t, device device, std::span<render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth, bool one_shot, std::string_view label) {
		{ T::create(device, colors, depth, /*is the resulting command buffer intended to only be used once?*/one_shot, label) } -> std::convertible_to<T>;
	};



	struct device {
		struct create_config {
			std::string_view label = "Stylizer Device";
			bool high_performance = true;
			struct surface* compatible_surface = nullptr; // When set to null the device will be configured in headless mode
		};

		virtual texture& create_texture(temporary_return_t, const texture::create_config& config = {}) = 0;
		virtual texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const texture::data_layout& layout, const texture::create_config& config = {}) = 0;
		virtual render_pass& create_render_pass(temporary_return_t, std::span<render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, std::string_view label = "Stylizer Render Pass") = 0;

		virtual void release(bool static_sub_objects = false) = 0;
		STYLIZER_API_AS_METHOD(device);
	};

	template<typename T>
	concept device_concept = std::derived_from<T, device> && requires(T t, device::create_config config,
		texture::create_config texture_config,
		std::span<render_pass::color_attachment> colors, std::optional<render_pass::depth_stencil_attachment> depth, bool one_shot, std::string_view label
	) {
		{ T::create_default(config) } -> std::convertible_to<T>;

		{ t.create_texture(texture_config) } -> std::derived_from<texture>;
		{ t.create_render_pass(colors, depth, one_shot, label) } -> std::derived_from<render_pass>;
	};
}