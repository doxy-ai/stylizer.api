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

	/**
	 * @brief Defines the severity level of an error or log message.
	 */
	enum class error_severity {
		Verbose, ///< Detailed diagnostic information.
		Info,    ///< General information about system state.
		Warning, ///< Non-critical issues that don't stop execution.
		Error,   ///< Critical issues that may require immediate attention.
	};

	/**
	 * @brief Accesses the global error handler signal.
	 *
	 * Applications can connect to this signal to receive error and log messages from the library.
	 *
	 * @return A reference to the error handler signal.
	 *
	 * @code
	 * auto& errors = stylizer::get_error_handler();
	 * stylizer::auto_release c = errors.connect([](stylizer::error_severity severity, std::string_view message, size_t tag) {
	 *     std::cerr << "[" << (int)severity << "] " << message << std::endl;
	 * });
	 * @endcode
	 */
	stylizer::signal<void(error_severity, std::string_view message, size_t error_tag)>& get_error_handler();
}
/**
 * @brief Namespace containing the core Stylizer Graphics API.
 */
namespace stylizer::api {

	/** @brief A 32-bit boolean type, often used for GPU compatibility. */
	using bool32 = uint32_t;

	/** @brief A 2D vector of unsigned integers. */
	using vec2u = stdmath::vector<size_t, 2>;

	/** @brief A 3D vector of unsigned integers. */
	using vec3u = stdmath::vector<size_t, 3>;

	/** @brief A 4-component color using 32-bit floating point values (RGBA). */
	using color32 = stdmath::vector<stdmath::stl::float32_t, 4>;

	/** @brief A 4-component color using 8-bit unsigned integers (RGBA). */
	using color8 = stdmath::vector<uint8_t, 4>;

	/** @brief A vector of 32-bit unsigned integers representing SPIR-V bytecode. */
	using spirv = std::vector<uint32_t>;

	/** @brief A span of 32-bit unsigned integers representing SPIR-V bytecode. */
	using spirv_view = std::span<uint32_t>;

	/**
	 * @brief Exception class for Stylizer API errors.
	 */
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

	/**
	 * @brief Specifies how alpha transparency should be handled for a surface.
	 */
	enum class alpha_mode {
		Opaque,         ///< Alpha channel is ignored, surface is fully opaque.
		PostMultiplied, ///< Alpha is not multiplied into the color channels.
		PreMultiplied,  ///< Color channels are pre-multiplied by the alpha value.
		Inherit,        ///< Inherit the alpha mode from the environment.
	};

	/**
	 * @brief Defines the intended usage of a resource (buffer or texture).
	 *
	 * These flags can be combined using bitwise OR.
	 */
	enum class usage {
		Invalid = 0,
		CopySource = (1 << 0),          ///< Can be used as a source for copy operations.
		CopyDestination = (1 << 1),     ///< Can be used as a destination for copy operations.
		RenderAttachment = (1 << 2),    ///< Can be used as a color or depth/stencil attachment in a render pass.
		TransientAttachment = (1 << 3), ///< Can be used as a transient attachment.
		StorageAttachment = (1 << 4),   ///< Can be used as a storage attachment.
		Texture = (1 << 5),             ///< Can be used as a sampled texture in a shader.

		Storage = (1 << 8),             ///< Can be used as a storage buffer/texture.
		Uniform = (1 << 9),             ///< Can be used as a uniform buffer.
		Vertex = (1 << 6),              ///< Can be used as a vertex buffer.
		Index = (1 << 7),               ///< Can be used as an index buffer.
		MapRead = (1 << 10),            ///< Can be mapped for reading on the CPU.
		MapWrite = (1 << 11),           ///< Can be mapped for writing on the CPU.
	};

	/**
	 * @brief Languages used for shader source code.
	 */
	enum class shader_language {
		SPIRV, ///< SPIR-V binary format.
		GLSL,  ///< OpenGL Shading Language.
		Slang, ///< Slang shading language.
		WGSL,  ///< WebGPU Shading Language.
	};

	/**
	 * @brief Shader stages in the graphics or compute pipeline.
	 */
	enum class shader_stage {
		Combined, ///< Combined stages (mostly used for WGSL where multiple stages can be in one file).
		Vertex,   ///< Vertex shader stage.
		Fragment, ///< Fragment (pixel) shader stage.
		Compute,  ///< Compute shader stage.
	};

	/**
	 * @brief Comparison functions for depth and stencil tests.
	 */
	enum class comparison_function {
		NoDepthCompare, ///< No comparison is performed.
		NeverPass,      ///< Comparison never passes.
		Less,           ///< Passes if the new value is less than the existing value.
		LessEqual,      ///< Passes if the new value is less than or equal to the existing value.
		Greater,        ///< Passes if the new value is greater than the existing value.
		GreaterEqual,   ///< Passes if the new value is greater than or equal to the existing value.
		Equal,          ///< Passes if the new value is equal to the existing value.
		NotEqual,       ///< Passes if the new value is not equal to the existing value.
		AlwaysPass,     ///< Comparison always passes.
	};

	/**
	 * @brief Configuration for color blending.
	 */
	struct blend_state {
		/** @brief Blending operations. */
		enum class operation {
			Add,             ///< Add source and destination.
			Subtract,        ///< Subtract source from destination.
			ReverseSubtract, ///< Subtract destination from source.
			Min,             ///< Minimum of source and destination.
			Max,             ///< Maximum of source and destination.
		} operation = blend_state::operation::Add;

		/** @brief Blending factors. */
		enum class factor {
			Zero,                  ///< Factor is (0, 0, 0, 0).
			One,                   ///< Factor is (1, 1, 1, 1).
			Source,                ///< Factor is source color.
			OneMinusSource,        ///< Factor is 1 - source color.
			SourceAlpha,           ///< Factor is source alpha.
			OneMinusSourceAlpha,   ///< Factor is 1 - source alpha.
			Destination,           ///< Factor is destination color.
			OneMinusDestination,   ///< Factor is 1 - destination color.
			DestinationAlpha,      ///< Factor is destination alpha.
			OneMinusDestinationAlpha, ///< Factor is 1 - destination alpha.
			SourceAlphaSaturated,  ///< Factor is min(source alpha, 1 - destination alpha).
			Constant,              ///< Factor is constant color.
			OneMinusConstant,      ///< Factor is 1 - constant color.
		} source_factor = blend_state::factor::One, destination_factor = blend_state::factor::Zero;
	};

	/**
	 * @brief Defines a color attachment for a render pass.
	 */
	struct color_attachment {
		/** @brief The texture to use as an attachment. */
		STYLIZER_NULLABLE struct texture* texture = nullptr;

		/** @brief The texture view to use as an attachment (optional, defaults to full view of texture). */
		STYLIZER_NULLABLE struct texture_view* view = nullptr;

		/** @brief The format of the texture. */
		enum texture_format texture_format = texture_format::Undefined;

		/** @brief Optional resolve target for multi-sampled attachments. */
		STYLIZER_NULLABLE struct texture_view* resolve_target = nullptr;

		/** @brief Whether the content of the attachment should be stored after the pass. */
		bool should_store = true;

		/** @brief The clear color value. If not set, the attachment will not be cleared. */
		std::optional<color32> clear_value = {};

		/** @brief Blend state for the color channels. */
		std::optional<blend_state> color_blend_state = {};

		/** @brief Blend state for the alpha channel. */
		std::optional<blend_state> alpha_blend_state = {};
	};

	/**
	 * @brief Defines a depth/stencil attachment for a render pass.
	 */
	struct depth_stencil_attachment {
		/** @brief Configuration for stencil operations. */
		struct stencil_config {
			bool should_store = true; ///< Whether to store stencil data after the pass.

			std::optional<size_t> clear_value = {}; ///< Clear value for the stencil buffer.

			bool readonly = false; ///< Whether the stencil buffer is read-only.

			/** @brief Stencil face state for front/back faces. */
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

		/** @brief The texture to use as a depth/stencil attachment. */
		STYLIZER_NULLABLE struct texture* texture = nullptr;

		/** @brief The texture view to use as a depth/stencil attachment. */
		STYLIZER_NULLABLE struct texture_view* view = nullptr;

		/** @brief The format of the depth/stencil texture. */
		enum texture_format texture_format = texture_format::Undefined;

		/** @brief Whether the depth data should be stored after the pass. */
		bool should_store_depth = true;

		/** @brief The clear value for the depth buffer. */
		std::optional<float> depth_clear_value = {};

		/** @brief Whether the depth buffer is read-only. */
		bool depth_readonly = false;

		/** @brief The comparison function for the depth test. */
		comparison_function depth_comparison_function = comparison_function::Less;

		/** @brief Depth bias constant factor. */
		size_t depth_bias = 0;

		/** @brief Depth bias slope scale. */
		float depth_bias_slope_scale = 0;

		/** @brief Depth bias clamp value. */
		float depth_bias_clamp = 0;

		/** @brief Optional stencil configuration. */
		std::optional<stencil_config> stencil = {};
	};

	struct temporary_return_t {};

	constexpr static temporary_return_t temporary_return;

	struct device;

	/**
	 * @brief Represents a surface for rendering (e.g., a window or a canvas).
	 *
	 * Surfaces are used to present rendered images to the screen.
	 *
	 * @code
	 * surface.configure(device, surface.determine_optimal_default_config(device, {800, 600}));
	 * auto& texture = surface.next_texture(stylizer::api::temporary_return, device);
	 * // ... render to texture ...
	 * surface.present(device);
	 * @endcode
	 */
	struct surface {
		/** @brief Exception thrown when texture acquisition fails. */
		struct texture_acquisition_failed : public error {
			using error::error;
		};

		/** @brief Presentation modes for the surface. */
		enum class present_mode {
			Fifo = 0,      ///< Wait for vertical blank (VSync).
			FifoRelaxed,   ///< Similar to Fifo, but may tear if frame is late.
			Immediate,     ///< Present immediately, may cause tearing.
			Mailbox,       ///< Low latency, always presents the latest complete frame.
		};

		/** @brief Configuration for the surface. */
		struct config {
			enum present_mode present_mode = surface::present_mode::Fifo; ///< Presentation mode (e.g., VSync).
			enum texture_format texture_format = texture_format::RGBAu8_NormalizedSRGB; ///< Desired texture format.
			enum alpha_mode alpha_mode = alpha_mode::Opaque; ///< Alpha transparency mode.
			enum usage usage = usage::RenderAttachment; ///< Surface usage flags.
			vec2u size; ///< Dimensions of the surface.
		};

		/**
		 * @brief Determines an optimal configuration for the surface given a size.
		 *
		 * @param device The device to use for configuration.
		 * @param size The desired size of the surface.
		 * @return An optimal surface configuration.
		 */
		virtual config determine_optimal_default_config(device& device, vec2u size) {
			config out {};
			out.size = size;
			return out;
		}

		/**
		 * @brief Configures the surface with the given settings.
		 *
		 * @param device The device to use for configuration.
		 * @param config The configuration to apply.
		 * @return A reference to the surface.
		 */
		virtual surface& configure(device& device, const config& config) = 0;

		/**
		 * @brief Acquires the next available texture from the surface for rendering.
		 *
		 * @param device The device to use for acquisition.
		 * @return A reference to the acquired texture.
		 */
		virtual struct texture& next_texture(temporary_return_t, device& device) = 0;

		/**
		 * @brief Gets the texture format currently configured for the surface.
		 *
		 * @param device The device to use.
		 * @return The current texture format.
		 */
		virtual texture_format configured_texture_format(device& device);

		/**
		 * @brief Presents the current frame to the surface.
		 *
		 * @param device The device to use for presentation.
		 * @return A reference to the surface.
		 */
		virtual surface& present(device& device) = 0;

		/** @brief Checks if the surface is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the surface resources. */
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

	/**
	 * @brief Represents a specific view into a texture.
	 *
	 * Texture views can be used to select a subset of mip levels or specify a different aspect (e.g., depth-only).
	 */
	struct texture_view {
		/** @brief Configuration for creating a texture view. */
		struct create_config {
			size_t base_mip_level = 0;
			std::optional<size_t> mip_level_count_override = {};
			bool treat_as_cubemap = false;

			/** @brief Texture aspect to view. */
			enum class aspect {
				All = 0,
				DepthOnly,
				StencilOnly
			} aspect = aspect::All;
		};

		/** @brief Returns the texture that this view belongs to. */
		virtual const struct texture& texture() const = 0;

		/** @brief Conversion operator to the underlying texture. */
		operator const struct texture&() const { return texture(); }

		/** @brief Checks if the texture view is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the texture view resources. */
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

	/**
	 * @brief Represents a GPU texture resource.
	 *
	 * @code
	 * stylizer::auto_release texture = device.create_texture({
	 *     .format = texture_format::RGBAu8_NormalizedSRGB,
	 *     .usage = usage::Texture | usage::CopyDestination,
	 *     .size = {800, 600, 1}
	 * });
	 * @endcode
	 */
	struct texture {
		using format = api::texture_format;
		using view = texture_view;

		/** @brief Configuration for creating a texture. */
		struct create_config {
			std::string_view label = "Stylizer Texture"; ///< Debug label for the texture.
			enum texture_format format = format::RGBAu8_NormalizedSRGB; ///< Format of the texture.
			enum usage usage = usage::Texture; ///< Usage flags for the texture.
			vec3u size; ///< Dimensions of the texture (width, height, depth/layers).
			uint32_t mip_levels = 1; ///< Number of mip levels.
			uint32_t samples = 1; ///< Number of samples for multi-sampling.
			bool cubemap = false; ///< Whether the texture is a cubemap.
		};

		/** @brief Describes the layout of texture data in memory. */
		struct data_layout {
			size_t offset;
			size_t bytes_per_row;
			size_t rows_per_image;
		};

		/** @brief Sampler address modes. */
		enum class address_mode {
			Repeat,       ///< Repeat the texture.
			MirrorRepeat, ///< Repeat the texture mirrored.
			ClampToEdge,  ///< Clamp to the edge color.
		};

		/** @brief Configuration for a texture sampler. */
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

		/**
		 * @brief Creates a new view for this texture.
		 *
		 * @param device The device to use.
		 * @param config The configuration for the view.
		 * @return A reference to the created texture view.
		 */
		virtual texture_view& create_view(temporary_return_t, device& device, const view::create_config& config = {}) const = 0;

		/**
		 * @brief Returns a view covering the entire texture.
		 *
		 * @param device The device to use.
		 * @param treat_as_cubemap Whether to treat the texture as a cubemap.
		 * @return A reference to the full texture view.
		 */
		virtual const texture_view& full_view(device& device, bool treat_as_cubemap = false) const = 0;

		/** @brief Gets the size of the texture. */
		virtual vec3u size() const = 0;

		/** @brief Gets the format of the texture. */
		virtual format texture_format() const = 0;

		/** @brief Gets the usage flags of the texture. */
		virtual enum usage usage() const = 0;

		/** @brief Gets the number of mip levels in the texture. */
		virtual uint32_t mip_levels() const = 0;

		/** @brief Gets the number of samples in the texture. */
		virtual uint32_t samples() const = 0;

		/**
		 * @brief Configures the sampler for this texture.
		 *
		 * @param device The device to use.
		 * @param config The sampler configuration.
		 * @return A reference to the texture.
		 */
		virtual texture& configure_sampler(device& device, const sampler_config& config) = 0;

		/** @brief Checks if the texture is sampled. */
		virtual bool sampled() const = 0;

		/**
		 * @brief Writes data to the texture.
		 *
		 * @param device The device to use.
		 * @param data The data to write.
		 * @param layout The layout of the data in memory.
		 * @param extent The extent of the region to write.
		 * @param origin The origin of the region to write.
		 * @param mip_level The mip level to write to.
		 * @return A reference to the texture.
		 */
		virtual texture& write(device& device, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin = { { 0, 0, 0 } }, size_t mip_level = 0) = 0;

		/**
		 * @brief Copies data from another texture.
		 *
		 * @param device The device to use.
		 * @param source The source texture.
		 * @param destination_origin The origin in the destination texture.
		 * @param source_origin The origin in the source texture.
		 * @param extent_override The extent of the copy.
		 * @param min_mip_level The minimum mip level to copy.
		 * @param mip_levels_override The number of mip levels to copy.
		 * @return A reference to the texture.
		 */
		virtual texture& copy_from(device& device, const texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = {}, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		/**
		 * @brief Blits data from another texture.
		 *
		 * @param device The device to use.
		 * @param source The source texture.
		 * @param clear_value Optional clear value.
		 * @param render_pipeline_override Optional render pipeline override.
		 * @param vertex_count_override Optional vertex count override.
		 * @return A reference to the texture.
		 */
		virtual texture& blit_from(device& device, const texture& source, std::optional<color32> clear_value = {}, STYLIZER_NULLABLE struct render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) = 0;

		/**
		 * @brief Generates mipmaps for the texture.
		 *
		 * @param device The device to use.
		 * @param first_mip_level The first mip level to generate from.
		 * @param mip_levels_override The number of mip levels to generate.
		 * @return A reference to the texture.
		 */
		virtual texture& generate_mipmaps(device& device, std::optional<size_t> first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		/** @brief Checks if the texture is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the texture resources. */
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

	/**
	 * @brief Represents a GPU buffer resource.
	 *
	 * @code
	 * stylizer::auto_release buffer = device.create_buffer(
	 *     usage::Vertex | usage::CopyDestination,
	 *     1024
	 * );
	 * @endcode
	 */
	struct buffer {
		/**
		 * @brief Returns a singleton zero buffer for the given usage and size.
		 */
		virtual const buffer& get_zero_buffer_singleton(device& device, enum usage usage = usage::Storage, size_t minimum_size = 0, STYLIZER_NULLABLE buffer* just_released = nullptr) = 0;

		/**
		 * @brief Writes data to the buffer.
		 *
		 * @param device The device to use.
		 * @param data The data to write.
		 * @param offset The offset in the buffer to start writing.
		 * @return A reference to the buffer.
		 */
		virtual buffer& write(device& device, std::span<const std::byte> data, size_t offset = 0) = 0;

		/** @brief Gets the size of the buffer in bytes. */
		virtual size_t size() const = 0;
		
		/** @brief Gets the usage flags of the buffer. */
		virtual enum usage usage() const = 0;

		/**
		 * @brief Copies data from another buffer.
		 *
		 * @param device The device to use.
		 * @param source The source buffer.
		 * @param destination_offset The offset in the destination buffer.
		 * @param source_offset The offset in the source buffer.
		 * @param size_override The size of the region to copy.
		 * @return A reference to the buffer.
		 */
		virtual buffer& copy_from(device& device, const buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) = 0;

		/** @brief Checks if the buffer is currently mapped. */
		virtual bool is_mapped() const = 0;

		/**
		 * @brief Maps the buffer asynchronously for CPU access.
		 */
		virtual std::future<std::byte*> map_async(device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) = 0;

		/**
		 * @brief Maps the buffer synchronously for CPU access.
		 */
		virtual std::byte* map(device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) = 0;

		/** @brief Unmaps the buffer. */
		virtual void unmap() = 0;

		/** @brief Checks if the buffer is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the buffer resources. */
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

	/**
	 * @brief Represents a shader module.
	 *
	 * @code
	 * stylizer::auto_release shader = device.create_shader_from_source(
	 *     shader_language::WGSL,
	 *     shader_stage::Vertex,
	 *     "@vertex fn main() -> @builtin(position) vec4f { return vec4f(0.0, 0.0, 0.0, 1.0); }"
	 * );
	 * @endcode
	 */
	struct shader {
		using language = shader_language;
		using stage = shader_stage;

		/** @brief Converts Stylizer shader stage to SLCross shader stage. */
		static slcross::shader_stage to_slcross(stage stage) {
			switch (stage) {
			case shader_stage::Vertex: return slcross::shader_stage::vertex;
			case shader_stage::Fragment: return slcross::shader_stage::fragment;
			case shader_stage::Compute: return slcross::shader_stage::compute;
			default: STYLIZER_API_THROW("The combined stage is only valid for WGSL");
			}
		}

		/**
		 * @brief Creates a shader from source code.
		 */
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

		/** @brief Checks if the shader is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the shader resources. */
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

	/**
	 * @brief Base class for graphics and compute pipelines.
	 */
	struct pipeline {
		/** @brief Defines an entry point in a shader for a pipeline. */
		struct entry_point {
			STYLIZER_NULLABLE struct shader* shader = nullptr; ///< The shader module containing the entry point.
			const std::string_view entry_point_name = "main";   ///< The name of the entry point function.
		};

		using entry_points = std::unordered_map<shader::stage, entry_point>;
	};

	/**
	 * @brief Represents a recorded set of GPU commands.
	 */
	struct command_buffer {
		stylizer::signal<void()> deferred_to_release;

		/** @brief Registers a function to be called when the command buffer is released. */
		template<std::convertible_to<std::function<void>> Tfunc>
		command_buffer& defer(Tfunc&& func) {
			deferred_to_release.connect(std::move(func));
			return *this;
		}

		/**
		 * @brief Submits the command buffer to the GPU for execution.
		 *
		 * @param device The device to submit to.
		 * @param release Whether to release the command buffer after submission.
		 */
		virtual void submit(device& device, bool release = true) = 0;

		/** @brief Checks if the command buffer is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the command buffer resources. */
		virtual void release() = 0;

		virtual ~command_buffer() = default;

		STYLIZER_API_AS_METHOD(command_buffer);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(command_buffer);
	};

	/**
	 * @brief Represents a group of resources bound to a pipeline.
	 */
	struct bind_group {
		/** @brief Binding for a buffer resource. */
		struct buffer_binding {
			const struct buffer* buffer;
			size_t offset = 0;
			std::optional<size_t> size_override = {};
		};

		/** @brief Binding for a texture resource. */
		struct texture_binding {
			STYLIZER_NULLABLE const struct texture* texture = nullptr;
			STYLIZER_NULLABLE const struct texture_view* texture_view = nullptr;
			std::optional<bool> sampled_override = {};
		};

		/** @brief Variant type for any binding. */
		using binding = std::variant<buffer_binding, texture_binding>;

		/** @brief Checks if the bind group is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the bind group resources. */
		virtual void release() = 0;

		virtual ~bind_group() = default;

		STYLIZER_API_AS_METHOD(bind_group);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(bind_group);
	};

	/**
	 * @brief Represents a compute pipeline.
	 *
	 * @code
	 * auto& pipeline = device.create_compute_pipeline(temporary_return, {&shader, "main"});
	 * encoder.bind_compute_pipeline(device, pipeline)
	 *        .dispatch_workgroups(device, {1, 1, 1});
	 * @endcode
	 */
	struct compute_pipeline : public pipeline {
		using pass = struct command_encoder;

		/** @brief Creates a bind group for this pipeline. */
		virtual bind_group& create_bind_group(temporary_return_t, device& device, size_t index, std::span<const bind_group::binding> bindings) = 0;

		/** @brief Convenience method to dispatch a compute shader in a single call. */
		static void quick_dispatch(device& device, vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings = {});

		/** @brief Checks if the pipeline is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the pipeline resources. */
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

	/**
	 * @brief Base template for command encoders.
	 *
	 * Command encoders are used to record GPU commands into a command buffer.
	 *
	 * @tparam Treturn The type returned by fluent methods (e.g., `command_encoder&` or `render_pass&`).
	 */
	template<typename Treturn>
	struct command_encoder_base {
		using pipeline = compute_pipeline;

		/** @brief Registers a function to be called when the encoder is finished. */
		virtual Treturn& defer(std::function<void()>&& func) = 0;

		/** @brief Records a buffer-to-buffer copy command. */
		virtual Treturn& copy_buffer_to_buffer(device& device, buffer& destination, const buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) = 0;

		/** @brief Records a buffer-to-texture copy command. */
		virtual Treturn& copy_buffer_to_texture(device& device, buffer& destination, const texture& source, std::optional<size_t> destination_offset = 0, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		/** @brief Records a texture-to-buffer copy command. */
		virtual Treturn& copy_texture_to_buffer(device& device, texture& destination, const buffer& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<size_t> source_offset = 0, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		/** @brief Records a texture-to-texture copy command. */
		virtual Treturn& copy_texture_to_texture(device& device, texture& destination, const texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) = 0;

		/** @brief Binds a compute pipeline to the encoder. */
		virtual Treturn& bind_compute_pipeline(device& device, const compute_pipeline& pipeline, bool release_on_submit = false) = 0;

		/** @brief Binds a bind group to the compute pipeline. */
		virtual Treturn& bind_compute_group(device& device, const bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) = 0;

		/** @brief Records a compute dispatch command. */
		virtual Treturn& dispatch_workgroups(device& device, vec3u workgroups) = 0;

		/** @brief Finishes recording and returns a command buffer. */
		virtual command_buffer& end(temporary_return_t, device& device) = 0;

		/** @brief Finishes recording and submits the commands immediately. */
		virtual void one_shot_submit(device& device) = 0;

		/** @brief Checks if the encoder is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the encoder resources. */
		virtual void release() = 0;

		virtual ~command_encoder_base() = default;

		STYLIZER_API_AS_METHOD(command_encoder_base);

		STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(Treturn);
	};

	/**
	 * @brief A general-purpose command encoder.
	 */
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

	/**
	 * @brief Represents a render pass for recording graphics commands.
	 *
	 * @code
	 * device.create_render_pass(temporary_return, color_attachments, depth_attachment, true)
	 *       .bind_render_pipeline(device, pipeline)
	 *       .draw(device, 3)
	 *       .one_shot_submit(device);
	 * @endcode
	 */
	struct render_pass : public command_encoder_base<render_pass> {
		using blend_state = api::blend_state;
		using color_attachment = api::color_attachment;
		using depth_stencil_attachment = api::depth_stencil_attachment;
		using pipeline = struct render_pipeline;

		/** @brief Binds a render pipeline to the pass. */
		virtual render_pass& bind_render_pipeline(device& device, const render_pipeline& pipeline, bool release_on_submit = false) = 0;

		/** @brief Binds a bind group to the render pipeline. */
		virtual render_pass& bind_render_group(device& device, const bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) = 0;

		/** @brief Binds a vertex buffer to the specified slot. */
		virtual render_pass& bind_vertex_buffer(device& device, size_t slot, const buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) = 0;

		/** @brief Binds an index buffer. */
		virtual render_pass& bind_index_buffer(device& device, const buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) = 0;

		/** @brief Records a non-indexed draw command. */
		virtual render_pass& draw(device& device, size_t vertex_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_vertex = 0, size_t first_instance = 0) = 0;

		/** @brief Records an indexed draw command. */
		virtual render_pass& draw_indexed(device& device, size_t index_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_index = 0, std::optional<size_t> base_vertex = 0, size_t first_instance = 0) = 0;

		/** @brief Checks if the render pass is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the render pass resources. */
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

	/**
	 * @brief Represents a graphics pipeline.
	 *
	 * @code
	 * stylizer::auto_release pipeline = device.create_render_pipeline(
	 *     {
	 *         {shader_stage::Vertex, {&shader, "vertex"}},
	 *         {shader_stage::Fragment, {&shader, "fragment"}}
	 *     },
	 *     color_attachments
	 * );
	 * @endcode
	 */
	struct render_pipeline : public pipeline {
		using blend_state = api::blend_state;
		using color_attachment = api::color_attachment;
		using depth_stencil_attachment = api::depth_stencil_attachment;
		using pass = render_pass;

		/** @brief Configuration for a render pipeline. */
		struct config {
			/** @brief Primitive topology. */
			enum class primitive_topology {
				PointList,
				LineList,
				LineStrip,
				TriangleList,
				TriangleStrip
			} primitive_topology = primitive_topology::TriangleList;

			float line_pixel_width = 1;

			/** @brief Cull mode. */
			enum class cull_mode {
				Back,
				Front,
				Both,
				Neither
			} cull_mode = cull_mode::Neither;

			bool winding_order_clockwise = false;

			bool u16_indices = false;

			/** @brief Defines the layout of a vertex buffer. */
			struct vertex_buffer_layout {
				/** @brief Defines a vertex attribute. */
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

			/** @brief Multi-sampling configuration. */
			struct sampling {
				uint32_t count = 1;
				uint32_t mask = ~0u;
				bool alpha_to_coverage = false;
			} multisampling = {};
		};

		/** @brief Creates a bind group for this pipeline. */
		virtual bind_group& create_bind_group(temporary_return_t, device& device, size_t index, std::span<const bind_group::binding> bindings) = 0;

		/** @brief Checks if the pipeline is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the pipeline resources. */
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

	/**
	 * @brief Represents a GPU device.
	 *
	 * The device is the primary interface for creating GPU resources and recording commands.
	 *
	 * @code
	 * stylizer::auto_release device = stylizer::api::current_backend::device::create_default();
	 * @endcode
	 */
	struct device {
		/** @brief Configuration for creating a device. */
		struct create_config {
			const std::string_view label = "Stylizer Device"; ///< Debug label for the device.
			const std::string_view queue_label = "Stylizer Queue"; ///< Debug label for the default queue.
			bool high_performance = true; ///< Request a high-performance adapter if available.
			STYLIZER_NULLABLE struct surface* compatible_surface = nullptr; ///< Optional surface the device should be compatible with.
		};

		/**
		 * @brief Ticks the device, processing events and optionally waiting for queues.
		 *
		 * @param wait_for_queues Whether to wait for all GPU queues to finish.
		 * @return true if successful.
		 */
		virtual bool tick(bool wait_for_queues = true) = 0;

		/** @brief Creates a new texture. */
		virtual texture& create_texture(temporary_return_t, const texture::create_config& config = {}) = 0;

		/** @brief Creates a new texture and initializes it with data. */
		virtual texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const texture::data_layout& layout, const texture::create_config& config = {}) = 0;

		/** @brief Creates a new buffer. */
		virtual buffer& create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") = 0;

		/** @brief Creates a new buffer and initializes it with data. */
		virtual buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") = 0;

		/** @brief Returns a singleton zero buffer. */
		virtual const buffer& get_zero_buffer_singleton(usage usage = usage::Storage, size_t minimum_size = 0, STYLIZER_NULLABLE buffer* just_released = nullptr) {
			auto& tmp = create_buffer(temporary_return, usage::Storage, 0);
			auto& out = tmp.get_zero_buffer_singleton(*this, usage, minimum_size, just_released);
			tmp.release();
			return out;
		}

		/** @brief Creates a shader from an SLCross session. */
		virtual shader& create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") = 0;

		/** @brief Creates a shader from SPIR-V bytecode. */
		virtual shader& create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") = 0;

		/** @brief Creates a shader from source code. */
		virtual shader& create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") = 0;

		/** @brief Creates a command encoder. */
		virtual command_encoder& create_command_encoder(temporary_return_t, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") = 0;

		/** @brief Creates a render pass. */
		virtual render_pass& create_render_pass(temporary_return_t, std::span<const render_pass::color_attachment> colors, const std::optional<render_pass::depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") = 0;

		/** @brief Creates a compute pipeline. */
		virtual compute_pipeline& create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") = 0;

		/** @brief Creates a render pipeline. */
		virtual render_pipeline& create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") = 0;

		/** @brief Creates a render pipeline compatible with a given render pass. */
		virtual render_pipeline& create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") = 0;

		/** @brief Convenience method for quick compute dispatch. */
		device& quick_compute_dispatch(vec3u workgroups, const pipeline::entry_point& entry_point, std::span<const bind_group::binding> bindings = {}) {
			compute_pipeline::quick_dispatch(*this, workgroups, entry_point, bindings);
			return *this;
		}

		/** @brief Checks if the device is valid. */
		virtual operator bool() const { return false; }

		/** @brief Releases the device resources. */
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

	/**
	 * @brief Helper to map C++ types to vertex buffer attribute formats.
	 */
	template<typename T>
	struct vertex_buffer_type_format {
		static constexpr auto format = render_pipeline::config::vertex_buffer_layout::attribute::format::Invalid;
	};

	/** @brief specialization for float. */
	template<>
	struct vertex_buffer_type_format<float> {
		static constexpr auto format = render_pipeline::config::vertex_buffer_layout::attribute::format::f32x1;
	};
}