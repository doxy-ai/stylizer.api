#ifndef STYLIZER_C_H
#define STYLIZER_C_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stylizer_device* stylizer_device_t;
typedef struct stylizer_surface* stylizer_surface_t;
typedef struct stylizer_texture* stylizer_texture_t;
typedef struct stylizer_texture_view* stylizer_texture_view_t;
typedef struct stylizer_buffer* stylizer_buffer_t;
typedef struct stylizer_shader* stylizer_shader_t;
typedef struct stylizer_bind_group* stylizer_bind_group_t;
typedef struct stylizer_compute_pipeline* stylizer_compute_pipeline_t;
typedef struct stylizer_render_pipeline* stylizer_render_pipeline_t;
typedef struct stylizer_command_encoder* stylizer_command_encoder_t;
typedef struct stylizer_render_pass* stylizer_render_pass_t;
typedef struct stylizer_command_buffer* stylizer_command_buffer_t;

typedef enum {
	STYLIZER_TEXTURE_FORMAT_UNDEFINED,

	STYLIZER_TEXTURE_FORMAT_RU8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RI8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RU8,
	STYLIZER_TEXTURE_FORMAT_RI8,
	STYLIZER_TEXTURE_FORMAT_RU16,
	STYLIZER_TEXTURE_FORMAT_RI16,
	STYLIZER_TEXTURE_FORMAT_RF16,
	STYLIZER_TEXTURE_FORMAT_RGU8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGI8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGU8,
	STYLIZER_TEXTURE_FORMAT_RGI8,
	STYLIZER_TEXTURE_FORMAT_RF32,
	STYLIZER_TEXTURE_FORMAT_RU32,
	STYLIZER_TEXTURE_FORMAT_RI32,
	STYLIZER_TEXTURE_FORMAT_RGU16,
	STYLIZER_TEXTURE_FORMAT_RGI16,
	STYLIZER_TEXTURE_FORMAT_RGF16,
	STYLIZER_TEXTURE_FORMAT_RGBAU8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGBAU8_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_RGBAI8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGBAU8,
	STYLIZER_TEXTURE_FORMAT_RGBAI8,
	STYLIZER_TEXTURE_FORMAT_BGRAU8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BGRAU8_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_RGBU10AU2,
	STYLIZER_TEXTURE_FORMAT_RGBU10AU2_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGF11BF10,
	STYLIZER_TEXTURE_FORMAT_RGF32,
	STYLIZER_TEXTURE_FORMAT_RGU32,
	STYLIZER_TEXTURE_FORMAT_RGI32,
	STYLIZER_TEXTURE_FORMAT_RGBAU16,
	STYLIZER_TEXTURE_FORMAT_RGBAI16,
	STYLIZER_TEXTURE_FORMAT_RGBAF16,
	STYLIZER_TEXTURE_FORMAT_RGBAF32,
	STYLIZER_TEXTURE_FORMAT_RGBAU32,
	STYLIZER_TEXTURE_FORMAT_RGBAI32,

	STYLIZER_TEXTURE_FORMAT_STENCIL_U8,
	STYLIZER_TEXTURE_FORMAT_DEPTH_U16,
	STYLIZER_TEXTURE_FORMAT_DEPTH_U24,
	STYLIZER_TEXTURE_FORMAT_DEPTH_U24_STENCIL_U8,
	STYLIZER_TEXTURE_FORMAT_DEPTH_F32,
	STYLIZER_TEXTURE_FORMAT_DEPTH_F32_STENCIL_U8,

	STYLIZER_TEXTURE_FORMAT_BC1RGBA_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC1RGBA_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_BC2RGBA_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC2RGBA_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_BC3RGBA_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC3RGBA_NORMALIZED_SRGB,

	STYLIZER_TEXTURE_FORMAT_BC4RU_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC4RI_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC5RGU_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC5RGI_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC6HRGB_UFLOAT,
	STYLIZER_TEXTURE_FORMAT_BC6HRGB_FLOAT,

	STYLIZER_TEXTURE_FORMAT_BC7RGBA_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_BC7RGBA_NORMALIZED_SRGB,

	STYLIZER_TEXTURE_FORMAT_ETC2RGB8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ETC2RGB8_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ETC2RGB8A1_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ETC2RGB8A1_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ETC2RGBA8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ETC2RGBA8_NORMALIZED_SRGB,

	STYLIZER_TEXTURE_FORMAT_EACRU11_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_EACRI11_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_EACRGU11_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_EACRGI11_NORMALIZED,

	STYLIZER_TEXTURE_FORMAT_ASTC4X4_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC4X4_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC5X4_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC5X4_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC5X5_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC5X5_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC6X5_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC6X5_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC6X6_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC6X6_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC8X5_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC8X5_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC8X6_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC8X6_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC8X8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC8X8_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC10X5_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC10X5_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC10X6_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC10X6_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC10X8_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC10X8_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC10X10_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC10X10_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC12X10_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC12X10_NORMALIZED_SRGB,
	STYLIZER_TEXTURE_FORMAT_ASTC12X12_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_ASTC12X12_NORMALIZED_SRGB,

	STYLIZER_TEXTURE_FORMAT_RU16_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGU16_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGBAU16_NORMALIZED,

	STYLIZER_TEXTURE_FORMAT_RI16_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGI16_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_RGBAI16_NORMALIZED,

	STYLIZER_TEXTURE_FORMAT_R8BG8_BIPLANAR420_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_R10X6BG10X6_BIPLANAR420_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_R8BG8A8_TRIPLANAR420_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_R8BG8_BIPLANAR422_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_R8BG8_BIPLANAR444_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_R10X6BG10X6_BIPLANAR422_NORMALIZED,
	STYLIZER_TEXTURE_FORMAT_R10X6BG10X6_BIPLANAR444_NORMALIZED,
} stylizer_texture_format_t;

typedef enum {
	STYLIZER_ALPHA_MODE_OPAQUE,
	STYLIZER_ALPHA_MODE_POST_MULTIPLIED,
	STYLIZER_ALPHA_MODE_PRE_MULTIPLIED,
	STYLIZER_ALPHA_MODE_INHERIT,
} stylizer_alpha_mode_t;

typedef enum {
	STYLIZER_USAGE_INVALID = 0,
	STYLIZER_USAGE_COPY_SOURCE = (1 << 0),
	STYLIZER_USAGE_COPY_DESTINATION = (1 << 1),
	STYLIZER_USAGE_RENDER_ATTACHMENT = (1 << 2),
	STYLIZER_USAGE_TRANSIENT_ATTACHMENT = (1 << 3),
	STYLIZER_USAGE_STORAGE_ATTACHMENT = (1 << 4),
	STYLIZER_USAGE_TEXTURE = (1 << 5),

	STYLIZER_USAGE_STORAGE = (1 << 8),
	STYLIZER_USAGE_UNIFORM = (1 << 9),
	STYLIZER_USAGE_VERTEX = (1 << 6),
	STYLIZER_USAGE_INDEX = (1 << 7),
	STYLIZER_USAGE_MAP_READ = (1 << 10),
	STYLIZER_USAGE_MAP_WRITE = (1 << 11),
} stylizer_usage_t;

typedef enum {
	STYLIZER_SHADER_LANGUAGE_SPIRV,
	STYLIZER_SHADER_LANGUAGE_GLSL,
	STYLIZER_SHADER_LANGUAGE_SLANG,
	STYLIZER_SHADER_LANGUAGE_WGSL,
} stylizer_shader_language_t;

typedef enum {
	STYLIZER_SHADER_STAGE_COMBINED,
	STYLIZER_SHADER_STAGE_VERTEX,
	STYLIZER_SHADER_STAGE_FRAGMENT,
	STYLIZER_SHADER_STAGE_COMPUTE,
} stylizer_shader_stage_t;

typedef enum {
	STYLIZER_COMPARISON_FUNCTION_NO_DEPTH_COMPARE,
	STYLIZER_COMPARISON_FUNCTION_NEVER_PASS,
	STYLIZER_COMPARISON_FUNCTION_LESS,
	STYLIZER_COMPARISON_FUNCTION_LESS_EQUAL,
	STYLIZER_COMPARISON_FUNCTION_GREATER,
	STYLIZER_COMPARISON_FUNCTION_GREATER_EQUAL,
	STYLIZER_COMPARISON_FUNCTION_EQUAL,
	STYLIZER_COMPARISON_FUNCTION_NOT_EQUAL,
	STYLIZER_COMPARISON_FUNCTION_ALWAYS_PASS,
} stylizer_comparison_function_t;

typedef struct {
	size_t x, y;
} stylizer_vec2u_t;

typedef struct {
	size_t x, y, z;
} stylizer_vec3u_t;

typedef struct {
	uint8_t r, g, b, a;
} stylizer_color8_t;

typedef struct {
	float r, g, b, a;
} stylizer_color32_t;

typedef enum {
	STYLIZER_BLEND_OPERATION_ADD,
	STYLIZER_BLEND_OPERATION_SUBTRACT,
	STYLIZER_BLEND_OPERATION_REVERSE_SUBTRACT,
	STYLIZER_BLEND_OPERATION_MIN,
	STYLIZER_BLEND_OPERATION_MAX,
} stylizer_blend_operation_t;

typedef enum {
	STYLIZER_BLEND_FACTOR_ZERO,
	STYLIZER_BLEND_FACTOR_ONE,
	STYLIZER_BLEND_FACTOR_SOURCE,
	STYLIZER_BLEND_FACTOR_ONE_MINUS_SOURCE,
	STYLIZER_BLEND_FACTOR_SOURCE_ALPHA,
	STYLIZER_BLEND_FACTOR_ONE_MINUS_SOURCE_ALPHA,
	STYLIZER_BLEND_FACTOR_DESTINATION,
	STYLIZER_BLEND_FACTOR_ONE_MINUS_DESTINATION,
	STYLIZER_BLEND_FACTOR_DESTINATION_ALPHA,
	STYLIZER_BLEND_FACTOR_ONE_MINUS_DESTINATION_ALPHA,
	STYLIZER_BLEND_FACTOR_SOURCE_ALPHA_SATURATED,
	STYLIZER_BLEND_FACTOR_CONSTANT,
	STYLIZER_BLEND_FACTOR_ONE_MINUS_CONSTANT,
} stylizer_blend_factor_t;

typedef struct {
	stylizer_blend_operation_t operation;
	stylizer_blend_factor_t source_factor;
	stylizer_blend_factor_t destination_factor;
} stylizer_blend_state_t;

typedef struct {
	stylizer_texture_t texture;
	stylizer_texture_view_t view;
	stylizer_texture_format_t texture_format;
	stylizer_texture_view_t resolve_target;
	bool should_store;
	bool has_clear_value;
	stylizer_color32_t clear_value;
	bool has_color_blend_state;
	stylizer_blend_state_t color_blend_state;
	bool has_alpha_blend_state;
	stylizer_blend_state_t alpha_blend_state;
} stylizer_color_attachment_t;

typedef struct {
	bool compare_always_pass; // placeholder for more detailed stencil state if needed
	// ... (omitting full stencil config for brevity unless really needed)
} stylizer_stencil_face_state_t;

typedef struct {
	bool should_store;
	bool has_clear_value;
	size_t clear_value;
	bool readonly;
	// ...
} stylizer_stencil_config_t;

typedef struct {
	stylizer_texture_t texture;
	stylizer_texture_view_t view;
	stylizer_texture_format_t texture_format;
	bool should_store_depth;
	bool has_depth_clear_value;
	float depth_clear_value;
	bool depth_readonly;
	stylizer_comparison_function_t depth_comparison_function;
	size_t depth_bias;
	float depth_bias_slope_scale;
	float depth_bias_clamp;
	bool has_stencil;
	stylizer_stencil_config_t stencil;
} stylizer_depth_stencil_attachment_t;

typedef enum {
	STYLIZER_PRESENT_MODE_FIFO = 0,
	STYLIZER_PRESENT_MODE_FIFO_RELAXED,
	STYLIZER_PRESENT_MODE_IMMEDIATE,
	STYLIZER_PRESENT_MODE_MAILBOX,
} stylizer_present_mode_t;

typedef struct {
	stylizer_present_mode_t present_mode;
	stylizer_texture_format_t texture_format;
	stylizer_alpha_mode_t alpha_mode;
	stylizer_usage_t usage;
	stylizer_vec2u_t size;
} stylizer_surface_config_t;

typedef struct {
	size_t base_mip_level;
	bool has_mip_level_count_override;
	size_t mip_level_count_override;
	bool treat_as_cubemap;
	enum {
		STYLIZER_TEXTURE_VIEW_ASPECT_ALL = 0,
		STYLIZER_TEXTURE_VIEW_ASPECT_DEPTH_ONLY,
		STYLIZER_TEXTURE_VIEW_ASPECT_STENCIL_ONLY
	} aspect;
} stylizer_texture_view_create_config_t;

typedef struct {
	const char* label;
	stylizer_texture_format_t format;
	stylizer_usage_t usage;
	stylizer_vec3u_t size;
	uint32_t mip_levels;
	uint32_t samples;
	bool cubemap;
} stylizer_texture_create_config_t;

typedef struct {
	size_t offset;
	size_t bytes_per_row;
	size_t rows_per_image;
} stylizer_texture_data_layout_t;

typedef enum {
	STYLIZER_ADDRESS_MODE_REPEAT,
	STYLIZER_ADDRESS_MODE_MIRROR_REPEAT,
	STYLIZER_ADDRESS_MODE_CLAMP_TO_EDGE,
} stylizer_address_mode_t;

typedef struct {
	const char* label;
	stylizer_address_mode_t mode;
	bool has_mode_x_override;
	stylizer_address_mode_t mode_x_override;
	bool has_mode_y_override;
	stylizer_address_mode_t mode_y_override;
	bool has_mode_z_override;
	stylizer_address_mode_t mode_z_override;
	bool magnify_linear;
	bool minify_linear;
	bool mip_linear;
	float lod_min_clamp;
	float lod_max_clamp;
	size_t max_anisotropy;
	stylizer_comparison_function_t depth_comparison_function;
} stylizer_texture_sampler_config_t;

typedef struct {
	stylizer_buffer_t buffer;
	size_t offset;
	bool has_size_override;
	size_t size_override;
} stylizer_bind_group_buffer_binding_t;

typedef struct {
	stylizer_texture_t texture;
	stylizer_texture_view_t texture_view;
	bool has_sampled_override;
	bool sampled_override;
} stylizer_bind_group_texture_binding_t;

typedef enum {
	STYLIZER_BINDING_TYPE_BUFFER,
	STYLIZER_BINDING_TYPE_TEXTURE,
} stylizer_binding_type_t;

typedef struct {
	stylizer_binding_type_t type;
	union {
		stylizer_bind_group_buffer_binding_t buffer;
		stylizer_bind_group_texture_binding_t texture;
	} binding;
} stylizer_bind_group_binding_t;

typedef struct {
	stylizer_shader_t shader;
	const char* entry_point_name;
} stylizer_pipeline_entry_point_t;

typedef struct {
	stylizer_shader_stage_t stage;
	stylizer_pipeline_entry_point_t entry_point;
} stylizer_pipeline_entry_points_element_t;

typedef enum {
	STYLIZER_PRIMITIVE_TOPOLOGY_POINT_LIST,
	STYLIZER_PRIMITIVE_TOPOLOGY_LINE_LIST,
	STYLIZER_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	STYLIZER_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	STYLIZER_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
} stylizer_primitive_topology_t;

typedef enum {
	STYLIZER_CULL_MODE_BACK,
	STYLIZER_CULL_MODE_FRONT,
	STYLIZER_CULL_MODE_BOTH,
	STYLIZER_CULL_MODE_NEITHER
} stylizer_cull_mode_t;

typedef enum {
	STYLIZER_VERTEX_FORMAT_INVALID,
	STYLIZER_VERTEX_FORMAT_F32X1,
	STYLIZER_VERTEX_FORMAT_F32X2,
	STYLIZER_VERTEX_FORMAT_F32X3,
	STYLIZER_VERTEX_FORMAT_F32X4,
	STYLIZER_VERTEX_FORMAT_F16X2,
	STYLIZER_VERTEX_FORMAT_F16X4,
	STYLIZER_VERTEX_FORMAT_I32X1,
	STYLIZER_VERTEX_FORMAT_I32X2,
	STYLIZER_VERTEX_FORMAT_I32X3,
	STYLIZER_VERTEX_FORMAT_I32X4,
	STYLIZER_VERTEX_FORMAT_I16X2,
	STYLIZER_VERTEX_FORMAT_I16X4,
	STYLIZER_VERTEX_FORMAT_I16X2_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_I16X4_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_I8X2,
	STYLIZER_VERTEX_FORMAT_I8X4,
	STYLIZER_VERTEX_FORMAT_I8X2_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_I8X4_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_U32X1,
	STYLIZER_VERTEX_FORMAT_U32X2,
	STYLIZER_VERTEX_FORMAT_U32X3,
	STYLIZER_VERTEX_FORMAT_U32X4,
	STYLIZER_VERTEX_FORMAT_U16X2,
	STYLIZER_VERTEX_FORMAT_U16X4,
	STYLIZER_VERTEX_FORMAT_U16X2_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_U16X4_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_U8X2,
	STYLIZER_VERTEX_FORMAT_U8X4,
	STYLIZER_VERTEX_FORMAT_U8X2_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_U8X4_NORMALIZED,
	STYLIZER_VERTEX_FORMAT_U10_10_10_2_NORMALIZED,
} stylizer_vertex_format_t;

typedef struct {
	stylizer_vertex_format_t format;
	bool has_offset_override;
	size_t offset_override;
	bool has_shader_location_override;
	size_t shader_location_override;
} stylizer_vertex_attribute_t;

typedef struct {
	const stylizer_vertex_attribute_t* attributes;
	size_t attribute_count;
	bool per_instance;
	bool has_stride_override;
	size_t stride_override;
} stylizer_vertex_buffer_layout_t;

typedef struct {
	stylizer_primitive_topology_t primitive_topology;
	float line_pixel_width;
	stylizer_cull_mode_t cull_mode;
	bool winding_order_clockwise;
	bool u16_indices;
	const stylizer_vertex_buffer_layout_t* vertex_buffers;
	size_t vertex_buffer_count;
	struct {
		uint32_t count;
		uint32_t mask;
		bool alpha_to_coverage;
	} multisampling;
} stylizer_render_pipeline_config_t;

// Device creation
stylizer_device_t stylizer_webgpu_device_create_default();
stylizer_device_t stylizer_current_backend_device_create_default();

// Surface creation (WebGPU)
stylizer_surface_t stylizer_webgpu_surface_create_from_x11(void* display, void* window);
stylizer_surface_t stylizer_webgpu_surface_create_from_wayland(void* display, void* surface);
stylizer_surface_t stylizer_webgpu_surface_create_from_cocoa(void* layer, void* window);
stylizer_surface_t stylizer_webgpu_surface_create_from_win32(void* window, void* instance);
stylizer_surface_t stylizer_webgpu_surface_create_from_emscripten(const char* canvas_id);

// Surface creation (Current Backend)
stylizer_surface_t stylizer_current_backend_surface_create_from_x11(void* display, void* window);
stylizer_surface_t stylizer_current_backend_surface_create_from_wayland(void* display, void* surface);
stylizer_surface_t stylizer_current_backend_surface_create_from_cocoa(void* layer, void* window);
stylizer_surface_t stylizer_current_backend_surface_create_from_win32(void* window, void* instance);
stylizer_surface_t stylizer_current_backend_surface_create_from_emscripten(const char* canvas_id);

// Generic Device methods
void stylizer_device_release(stylizer_device_t device);
bool stylizer_device_tick(stylizer_device_t device, bool wait_for_queues);
stylizer_texture_t stylizer_device_create_texture(stylizer_device_t device, const stylizer_texture_create_config_t* config);
stylizer_texture_t stylizer_device_create_and_write_texture(stylizer_device_t device, const void* data, size_t data_size, const stylizer_texture_data_layout_t* layout, const stylizer_texture_create_config_t* config);
stylizer_buffer_t stylizer_device_create_buffer(stylizer_device_t device, stylizer_usage_t usage, size_t size, bool mapped_at_creation, const char* label);
stylizer_buffer_t stylizer_device_create_and_write_buffer(stylizer_device_t device, stylizer_usage_t usage, const void* data, size_t data_size, size_t offset, const char* label);
stylizer_shader_t stylizer_device_create_shader_from_spirv(stylizer_device_t device, stylizer_shader_stage_t stage, const uint32_t* spirv, size_t spirv_count, const char* entry_point, const char* label);
stylizer_shader_t stylizer_device_create_shader_from_source(stylizer_device_t device, stylizer_shader_language_t lang, stylizer_shader_stage_t stage, const char* source, const char* entry_point, const char* label);
stylizer_command_encoder_t stylizer_device_create_command_encoder(stylizer_device_t device, bool one_shot, const char* label);
stylizer_render_pass_t stylizer_device_create_render_pass(stylizer_device_t device, const stylizer_color_attachment_t* colors, size_t color_count, const stylizer_depth_stencil_attachment_t* depth, bool one_shot, const char* label);
stylizer_compute_pipeline_t stylizer_device_create_compute_pipeline(stylizer_device_t device, const stylizer_pipeline_entry_point_t* entry_point, const char* label);
stylizer_render_pipeline_t stylizer_device_create_render_pipeline(stylizer_device_t device, const stylizer_pipeline_entry_points_element_t* entry_points, size_t entry_point_count, const stylizer_color_attachment_t* color_attachments, size_t color_attachment_count, const stylizer_depth_stencil_attachment_t* depth_attachment, const stylizer_render_pipeline_config_t* config, const char* label);

// Surface methods
void stylizer_surface_release(stylizer_surface_t surface);
void stylizer_surface_configure(stylizer_surface_t surface, stylizer_device_t device, const stylizer_surface_config_t* config);
stylizer_texture_t stylizer_surface_next_texture(stylizer_surface_t surface, stylizer_device_t device);
stylizer_texture_format_t stylizer_surface_configured_texture_format(stylizer_surface_t surface, stylizer_device_t device);
void stylizer_surface_present(stylizer_surface_t surface, stylizer_device_t device);

// Texture methods
void stylizer_texture_release(stylizer_texture_t texture);
stylizer_texture_view_t stylizer_texture_create_view(stylizer_texture_t texture, stylizer_device_t device, const stylizer_texture_view_create_config_t* config);
stylizer_vec3u_t stylizer_texture_get_size(stylizer_texture_t texture);
stylizer_texture_format_t stylizer_texture_get_format(stylizer_texture_t texture);
stylizer_usage_t stylizer_texture_get_usage(stylizer_texture_t texture);
void stylizer_texture_write(stylizer_texture_t texture, stylizer_device_t device, const void* data, size_t data_size, const stylizer_texture_data_layout_t* layout, stylizer_vec3u_t extent, const stylizer_vec3u_t* origin, size_t mip_level);
void stylizer_texture_copy_from(stylizer_texture_t texture, stylizer_device_t device, stylizer_texture_t source, const stylizer_vec3u_t* destination_origin, const stylizer_vec3u_t* source_origin, const stylizer_vec3u_t* extent_override, const size_t* min_mip_level, const size_t* mip_levels_override);
void stylizer_texture_generate_mipmaps(stylizer_texture_t texture, stylizer_device_t device, const size_t* first_mip_level, const size_t* mip_levels_override);

// TextureView methods
void stylizer_texture_view_release(stylizer_texture_view_t view);

// Buffer methods
void stylizer_buffer_release(stylizer_buffer_t buffer);
void stylizer_buffer_write(stylizer_buffer_t buffer, stylizer_device_t device, const void* data, size_t data_size, size_t offset);
size_t stylizer_buffer_get_size(stylizer_buffer_t buffer);
stylizer_usage_t stylizer_buffer_get_usage(stylizer_buffer_t buffer);
void* stylizer_buffer_map(stylizer_buffer_t buffer, stylizer_device_t device, bool for_writing, size_t offset, size_t size);
void stylizer_buffer_unmap(stylizer_buffer_t buffer);

// Shader methods
void stylizer_shader_release(stylizer_shader_t shader);

// BindGroup methods
void stylizer_bind_group_release(stylizer_bind_group_t bind_group);

// ComputePipeline methods
void stylizer_compute_pipeline_release(stylizer_compute_pipeline_t pipeline);
stylizer_bind_group_t stylizer_compute_pipeline_create_bind_group(stylizer_compute_pipeline_t pipeline, stylizer_device_t device, size_t index, const stylizer_bind_group_binding_t* bindings, size_t binding_count, const char* label);

// RenderPipeline methods
void stylizer_render_pipeline_release(stylizer_render_pipeline_t pipeline);
stylizer_bind_group_t stylizer_render_pipeline_create_bind_group(stylizer_render_pipeline_t pipeline, stylizer_device_t device, size_t index, const stylizer_bind_group_binding_t* bindings, size_t binding_count, const char* label);

// CommandEncoder methods
void stylizer_command_encoder_release(stylizer_command_encoder_t encoder);
void stylizer_command_encoder_copy_buffer_to_buffer(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_buffer_t destination, stylizer_buffer_t source, const size_t* destination_offset, const size_t* source_offset, const size_t* size_override);
void stylizer_command_encoder_bind_compute_pipeline(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_compute_pipeline_t pipeline, bool release_on_submit);
void stylizer_command_encoder_bind_compute_group(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_bind_group_t group, const bool* release_on_submit, const size_t* index_override);
void stylizer_command_encoder_dispatch_workgroups(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_vec3u_t workgroups);
stylizer_command_buffer_t stylizer_command_encoder_end(stylizer_command_encoder_t encoder, stylizer_device_t device);
void stylizer_command_encoder_one_shot_submit(stylizer_command_encoder_t encoder, stylizer_device_t device);

// RenderPass methods (inherits from CommandEncoder in C++ but we'll repeat or use cast)
void stylizer_render_pass_release(stylizer_render_pass_t pass);
void stylizer_render_pass_bind_render_pipeline(stylizer_render_pass_t pass, stylizer_device_t device, stylizer_render_pipeline_t pipeline, bool release_on_submit);
void stylizer_render_pass_bind_render_group(stylizer_render_pass_t pass, stylizer_device_t device, stylizer_bind_group_t group, const bool* release_on_submit, const size_t* index_override);
void stylizer_render_pass_bind_vertex_buffer(stylizer_render_pass_t pass, stylizer_device_t device, size_t slot, stylizer_buffer_t buffer, const size_t* offset, const size_t* size_override);
void stylizer_render_pass_bind_index_buffer(stylizer_render_pass_t pass, stylizer_device_t device, stylizer_buffer_t buffer, const size_t* offset, const size_t* size_override);
void stylizer_render_pass_draw(stylizer_render_pass_t pass, stylizer_device_t device, size_t vertex_count, const size_t* instance_count, const size_t* first_vertex, size_t first_instance);
void stylizer_render_pass_draw_indexed(stylizer_render_pass_t pass, stylizer_device_t device, size_t index_count, const size_t* instance_count, const size_t* first_index, const size_t* base_vertex, size_t first_instance);
stylizer_command_buffer_t stylizer_render_pass_end(stylizer_render_pass_t pass, stylizer_device_t device);

// CommandBuffer methods
void stylizer_command_buffer_release(stylizer_command_buffer_t buffer);
void stylizer_command_buffer_submit(stylizer_command_buffer_t buffer, stylizer_device_t device, bool release);

#ifdef __cplusplus
}
#endif

#endif
