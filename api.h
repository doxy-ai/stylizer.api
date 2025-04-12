#ifdef STYLIZER_API_IMPLEMENTATION
#include "api.hpp"

const char* std_string_to_c_string(const std::string& str) {
	const char* out = malloc(str.size());
	std::memcpy(out, str.c_str(), str.size());
	out[str.size() - 1] = '\0';
	return out;
}
#else
#include "c_view.h"
#include "optional.h"

typedef uint32_t stylizer_api_bool32;
typedef struct stylizer_api_vec2u { size_t x = 0, y = 0; } stylizer_api_vec2u;
typedef struct stylizer_api_vec3u { size_t x = 0, y = 0, z = 0; } stylizer_api_vec3u;
typedef struct stylizer_api_color32 { float r = 0, g = 0, b = 0, a = 1; } stylizer_api_color32;
typedef struct stylizer_api_color8 { uint8_t r = 0, g = 0, b = 0, a = 1; } stylizer_api_color8;
 
typedef struct stylizer_api_device stylizer_api_device;
typedef struct stylizer_api_surface stylizer_api_surface;
typedef struct stylizer_api_texture_view stylizer_api_texture_view;
typedef struct stylizer_api_texture stylizer_api_texture;
typedef struct stylizer_api_buffer stylizer_api_buffer;
typedef struct stylizer_api_shader stylizer_api_shader;
typedef struct stylizer_api_command_buffer stylizer_api_command_buffer;
typedef struct stylizer_api_bind_group stylizer_api_bind_group;
typedef struct stylizer_api_compute_pipeline stylizer_api_compute_pipeline;
typedef struct stylizer_api_command_encoder stylizer_api_command_encoder;
typedef struct stylizer_api_render_pass stylizer_api_render_pass;
typedef struct stylizer_api_render_pipeline stylizer_api_render_pipeline;

enum stylizer_api_alpha_mode {
	stylizer_api_Opaque,
	stylizer_api_PostMultiplied,
	stylizer_api_PreMultiplied,
	stylizer_api_Inherit,
};

#include "texture_format.h"

enum stylizer_api_usage {
	stylizer_api_Invalid = 0,
	stylizer_api_CopySource = (1 << 0),
	stylizer_api_CopyDestination = (1 << 1),
	stylizer_api_RenderAttachment = (1 << 2),
	stylizer_api_TextureBinding = (1 << 3),

	stylizer_api_Vertex = (1 << 4),
	stylizer_api_Index = (1 << 5),
	stylizer_api_Storage = (1 << 6),
	stylizer_api_Uniform = (1 << 7),
	stylizer_api_MapRead = (1 << 8),
	stylizer_api_MapWrite = (1 << 9),
};

enum shader_language {
	stylizer_api_SPIRV,
	stylizer_api_GLSL,
	stylizer_api_Slang,
	stylizer_api_WGSL,
};

enum stylizer_api_shader_stage {
	stylizer_api_Combined, // Only valid for WGSL
	stylizer_api_Vertex_Stage,
	stylizer_api_Fragment
};

enum comparison_function {
	stylizer_api_NoDepthCompare,
	stylizer_api_Never,
	stylizer_api_Less,
	stylizer_api_LessEqual,
	stylizer_api_Greater,
	stylizer_api_GreaterEqual,
	stylizer_api_Equal,
	stylizer_api_NotEqual,
	stylizer_api_Always,
};

enum stylizer_api_blend_state_operation {
	stylizer_api_blend_state_Add,
	stylizer_api_blend_state_Subtract,
	stylizer_api_blend_state_ReverseSubtract,
	stylizer_api_blend_state_Min,
	stylizer_api_blend_state_Max,
};

enum stylizer_api_blend_state_factor {
	stylizer_api_blend_state_Zero,
	stylizer_api_blend_state_One,
	stylizer_api_blend_state_Src,
	stylizer_api_blend_state_OneMinusSrc,
	stylizer_api_blend_state_SrcAlpha,
	stylizer_api_blend_state_OneMinusSrcAlpha,
	stylizer_api_blend_state_Dst,
	stylizer_api_blend_state_OneMinusDst,
	stylizer_api_blend_state_DstAlpha,
	stylizer_api_blend_state_OneMinusDstAlpha,
	stylizer_api_blend_state_SrcAlphaSaturated,
	stylizer_api_blend_state_Constant,
	stylizer_api_blend_state_OneMinusConstant,
};

struct stylizer_api_blend_state {
	stylizer_api_blend_state_operation operation;
	stylizer_api_blend_state_factor source_factor;
	stylizer_api_blend_state_factor destination_factor;
};

typedef struct stylizer_api_color_attachment {
	stylizer_api_texture* texture; stylizer_api_texture_view* view; stylizer_api_texture_format texture_format; // Only one of these need be specified, they will be calculated as needed (note if creating a pass only specifying the format will produce an error!)
	stylizer_api_texture_view* resolve_target; // Used for multisampling
	bool should_store; // False discards
	STYLIZER_OPTIONAL(stylizer_api_color32) clear_value; // When set value is not loaded
	STYLIZER_OPTIONAL(stylizer_api_blend_state) color_blend_state;
	STYLIZER_OPTIONAL(stylizer_api_blend_state) alpha_blend_state;
} stylizer_api_color_attachment;

stylizer_api_color_attachment stylizer_api_color_attachment_create_default() {
	stylizer::api::color_attachment def = {};
	return (stylizer_api_color_attachment&)def;
}

typedef struct stylizer_api_depth_stencil_attachment {
	stylizer_api_texture* texture; stylizer_api_texture_view* view; stylizer_api_texture_format texture_format; // Only one of these need be specified, they will be calculated as needed (note if creating a pass only specifying the format will produce an error!)
	bool should_store_depth; // False discards
	STYLIZER_OPTIONAL(float) depth_clear_value; // When set value is not loaded
	bool depth_readonly;
	comparison_function depth_comparison_function;
	size_t depth_bias;
	float depth_bias_slope_scale;
	float depth_bias_clamp;

	struct stencil_config {
		bool should_store; // False discards
		STYLIZER_OPTIONAL(size_t) clear_value; // When set value is not loaded // TODO: Should be uint32_t?
		bool readonly = false;
	};
	STYLIZER_OPTIONAL(stencil_config) stencil = {};
} stylizer_api_depth_stencil_attachment;

stylizer_api_depth_stencil_attachment stylizer_api_depth_stencil_attachment_create_default() {
	stylizer::api::depth_stencil_attachment def = {};
	return (stylizer_api_depth_stencil_attachment&)def;
}

struct stylizer_api_device_create_config {
	const char* label;
	bool high_performance;
	stylizer_api_surface* compatible_surface; // When set to null the device will be configured in headless mode
};

stylizer_api_device_create_config stylizer_api_device_create_config_create_default(){
	stylizer::api::device::create_config def = {};
	stylizer_api_device_create_config c;
	c.label = std_string_to_c_string(def.label);
	c.high_performance = def.high_performance;
	c.compatible_surface = def.compatible_surface;
	return c;
}

stylizer_api_device* stylizer_api_create_default_device_for_current_backend(stylizer_api_device_create_config* config) {
	return new stylizer::api::current_backend::device(stylizer::api::current_backend::device::create_default(*config));
}

stylizer_api_device* stylizer_api_create_default_webgpu_device(stylizer_api_device_create_config* config) {
	return new stylizer::api::webgpu::device(stylizer::api::webgpu::device::create_default(*config));
}

#endif