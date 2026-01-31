#include "stylizer.h"
#include "../api.hpp"
#include "../backends/webgpu/webgpu.hpp"
#include "../backends/current_backend.hpp"
#include <vector>
#include <algorithm>

using namespace stylizer;

// Cast helpers
#define CAST_DEVICE(d) (reinterpret_cast<api::device*>(d))
#define CAST_SURFACE(s) (reinterpret_cast<api::surface*>(s))
#define CAST_TEXTURE(t) (reinterpret_cast<api::texture*>(t))
#define CAST_TEXTURE_VIEW(tv) (reinterpret_cast<api::texture_view*>(tv))
#define CAST_BUFFER(b) (reinterpret_cast<api::buffer*>(b))
#define CAST_SHADER(s) (reinterpret_cast<api::shader*>(s))
#define CAST_BIND_GROUP(bg) (reinterpret_cast<api::bind_group*>(bg))
#define CAST_COMPUTE_PIPELINE(cp) (reinterpret_cast<api::compute_pipeline*>(cp))
#define CAST_RENDER_PIPELINE(rp) (reinterpret_cast<api::render_pipeline*>(rp))
#define CAST_COMMAND_ENCODER(ce) (reinterpret_cast<api::command_encoder*>(ce))
#define CAST_RENDER_PASS(rp) (reinterpret_cast<api::render_pass*>(rp))
#define CAST_COMMAND_BUFFER(cb) (reinterpret_cast<api::command_buffer*>(cb))

extern "C" {

// Device creation
stylizer_device_t stylizer_webgpu_device_create_default() {
    auto dev = api::webgpu::device::create_default();
    return reinterpret_cast<stylizer_device_t>(dev.move_temporary_to_heap());
}

stylizer_device_t stylizer_current_backend_device_create_default() {
#if STYLIZER_CURRENT_BACKEND == 1
    auto dev = api::current_backend::device::create_default();
    return reinterpret_cast<stylizer_device_t>(dev.move_temporary_to_heap());
#else
    return nullptr;
#endif
}

// Surface creation (WebGPU)
stylizer_surface_t stylizer_webgpu_surface_create_from_x11(void* display, void* window) {
    auto surf = api::webgpu::surface::create_from_x11(display, window);
    return reinterpret_cast<stylizer_surface_t>(surf.move_temporary_to_heap());
}
stylizer_surface_t stylizer_webgpu_surface_create_from_wayland(void* display, void* surface) {
    auto surf = api::webgpu::surface::create_from_wayland(display, surface);
    return reinterpret_cast<stylizer_surface_t>(surf.move_temporary_to_heap());
}
stylizer_surface_t stylizer_webgpu_surface_create_from_cocoa(void* layer, void* window) {
    auto surf = api::webgpu::surface::create_from_cocoa(layer, window);
    return reinterpret_cast<stylizer_surface_t>(surf.move_temporary_to_heap());
}
stylizer_surface_t stylizer_webgpu_surface_create_from_win32(void* window, void* instance) {
    auto surf = api::webgpu::surface::create_from_win32(window, instance);
    return reinterpret_cast<stylizer_surface_t>(surf.move_temporary_to_heap());
}
stylizer_surface_t stylizer_webgpu_surface_create_from_emscripten(const char* canvas_id) {
    auto surf = api::webgpu::surface::create_from_emscripten(canvas_id ? canvas_id : "canvas");
    return reinterpret_cast<stylizer_surface_t>(surf.move_temporary_to_heap());
}

// Surface creation (Current Backend)
stylizer_surface_t stylizer_current_backend_surface_create_from_x11(void* display, void* window) {
#if STYLIZER_CURRENT_BACKEND == 1
    return stylizer_webgpu_surface_create_from_x11(display, window);
#else
    return nullptr;
#endif
}
stylizer_surface_t stylizer_current_backend_surface_create_from_wayland(void* display, void* surface) {
#if STYLIZER_CURRENT_BACKEND == 1
    return stylizer_webgpu_surface_create_from_wayland(display, surface);
#else
    return nullptr;
#endif
}
stylizer_surface_t stylizer_current_backend_surface_create_from_cocoa(void* layer, void* window) {
#if STYLIZER_CURRENT_BACKEND == 1
    return stylizer_webgpu_surface_create_from_cocoa(layer, window);
#else
    return nullptr;
#endif
}
stylizer_surface_t stylizer_current_backend_surface_create_from_win32(void* window, void* instance) {
#if STYLIZER_CURRENT_BACKEND == 1
    return stylizer_webgpu_surface_create_from_win32(window, instance);
#else
    return nullptr;
#endif
}
stylizer_surface_t stylizer_current_backend_surface_create_from_emscripten(const char* canvas_id) {
#if STYLIZER_CURRENT_BACKEND == 1
    return stylizer_webgpu_surface_create_from_emscripten(canvas_id);
#else
    return nullptr;
#endif
}

// Generic Device methods
void stylizer_device_release(stylizer_device_t device) {
    if (device) {
        auto dev = CAST_DEVICE(device);
        dev->release();
        delete dev;
    }
}

bool stylizer_device_tick(stylizer_device_t device, bool wait_for_queues) {
    return CAST_DEVICE(device)->tick(wait_for_queues);
}

stylizer_texture_t stylizer_device_create_texture(stylizer_device_t device, const stylizer_texture_create_config_t* config) {
    api::texture::create_config cpp_config;
    if (config) {
        if (config->label) cpp_config.label = config->label;
        cpp_config.format = static_cast<api::texture_format>(config->format);
        cpp_config.usage = static_cast<api::usage>(config->usage);
        cpp_config.size = { config->size.x, config->size.y, config->size.z };
        cpp_config.mip_levels = config->mip_levels;
        cpp_config.samples = config->samples;
        cpp_config.cubemap = config->cubemap;
    }
    auto& tex = CAST_DEVICE(device)->create_texture(api::temporary_return, cpp_config);
    return reinterpret_cast<stylizer_texture_t>(tex.move_temporary_to_heap());
}

stylizer_texture_t stylizer_device_create_and_write_texture(stylizer_device_t device, const void* data, size_t data_size, const stylizer_texture_data_layout_t* layout, const stylizer_texture_create_config_t* config) {
    api::texture::create_config cpp_config;
    if (config) {
        if (config->label) cpp_config.label = config->label;
        cpp_config.format = static_cast<api::texture_format>(config->format);
        cpp_config.usage = static_cast<api::usage>(config->usage);
        cpp_config.size = { config->size.x, config->size.y, config->size.z };
        cpp_config.mip_levels = config->mip_levels;
        cpp_config.samples = config->samples;
        cpp_config.cubemap = config->cubemap;
    }
    api::texture::data_layout cpp_layout = { 0, 0, 0 };
    if (layout) {
        cpp_layout = { layout->offset, layout->bytes_per_row, layout->rows_per_image };
    }
    auto& tex = CAST_DEVICE(device)->create_and_write_texture(api::temporary_return, std::span<const std::byte>(static_cast<const std::byte*>(data), data_size), cpp_layout, cpp_config);
    return reinterpret_cast<stylizer_texture_t>(tex.move_temporary_to_heap());
}

stylizer_buffer_t stylizer_device_create_buffer(stylizer_device_t device, stylizer_usage_t usage, size_t size, bool mapped_at_creation, const char* label) {
    auto& buf = CAST_DEVICE(device)->create_buffer(api::temporary_return, static_cast<api::usage>(usage), size, mapped_at_creation, label ? label : "Stylizer Buffer");
    return reinterpret_cast<stylizer_buffer_t>(buf.move_temporary_to_heap());
}

stylizer_buffer_t stylizer_device_create_and_write_buffer(stylizer_device_t device, stylizer_usage_t usage, const void* data, size_t data_size, size_t offset, const char* label) {
    auto& buf = CAST_DEVICE(device)->create_and_write_buffer(api::temporary_return, static_cast<api::usage>(usage), std::span<const std::byte>(static_cast<const std::byte*>(data), data_size), offset, label ? label : "Stylizer Buffer");
    return reinterpret_cast<stylizer_buffer_t>(buf.move_temporary_to_heap());
}

stylizer_shader_t stylizer_device_create_shader_from_spirv(stylizer_device_t device, stylizer_shader_stage_t stage, const uint32_t* spirv, size_t spirv_count, const char* entry_point, const char* label) {
    auto& s = CAST_DEVICE(device)->create_shader_from_spirv(api::temporary_return, static_cast<api::shader_stage>(stage), api::spirv_view(const_cast<uint32_t*>(spirv), spirv_count), entry_point ? entry_point : "main", label ? label : "Stylizer Shader");
    return reinterpret_cast<stylizer_shader_t>(s.move_temporary_to_heap());
}

stylizer_shader_t stylizer_device_create_shader_from_source(stylizer_device_t device, stylizer_shader_language_t lang, stylizer_shader_stage_t stage, const char* source, const char* entry_point, const char* label) {
    auto& s = CAST_DEVICE(device)->create_shader_from_source(api::temporary_return, static_cast<api::shader_language>(lang), static_cast<api::shader_stage>(stage), source, entry_point ? std::optional<const std::string_view>(entry_point) : std::nullopt, label ? label : "Stylizer Shader");
    return reinterpret_cast<stylizer_shader_t>(s.move_temporary_to_heap());
}

stylizer_command_encoder_t stylizer_device_create_command_encoder(stylizer_device_t device, bool one_shot, const char* label) {
    auto& ce = CAST_DEVICE(device)->create_command_encoder(api::temporary_return, one_shot, label ? label : "Stylizer Command Encoder");
    return reinterpret_cast<stylizer_command_encoder_t>(ce.move_temporary_to_heap());
}

static api::blend_state to_cpp(const stylizer_blend_state_t& state) {
    return api::blend_state {
        .operation = static_cast<api::blend_state::operation>(state.operation),
        .source_factor = static_cast<api::blend_state::factor>(state.source_factor),
        .destination_factor = static_cast<api::blend_state::factor>(state.destination_factor)
    };
}

static api::color_attachment to_cpp(const stylizer_color_attachment_t& attachment) {
    api::color_attachment out;
    out.texture = CAST_TEXTURE(attachment.texture);
    out.view = CAST_TEXTURE_VIEW(attachment.view);
    out.texture_format = static_cast<api::texture_format>(attachment.texture_format);
    out.resolve_target = CAST_TEXTURE_VIEW(attachment.resolve_target);
    out.should_store = attachment.should_store;
    if (attachment.has_clear_value) {
        out.clear_value = stdmath::color32{ attachment.clear_value.r, attachment.clear_value.g, attachment.clear_value.b, attachment.clear_value.a };
    }
    if (attachment.has_color_blend_state) {
        out.color_blend_state = to_cpp(attachment.color_blend_state);
    }
    if (attachment.has_alpha_blend_state) {
        out.alpha_blend_state = to_cpp(attachment.alpha_blend_state);
    }
    return out;
}

static api::depth_stencil_attachment to_cpp(const stylizer_depth_stencil_attachment_t& attachment) {
    api::depth_stencil_attachment out;
    out.texture = CAST_TEXTURE(attachment.texture);
    out.view = CAST_TEXTURE_VIEW(attachment.view);
    out.texture_format = static_cast<api::texture_format>(attachment.texture_format);
    out.should_store_depth = attachment.should_store_depth;
    if (attachment.has_depth_clear_value) {
        out.depth_clear_value = attachment.depth_clear_value;
    }
    out.depth_readonly = attachment.depth_readonly;
    out.depth_comparison_function = static_cast<api::comparison_function>(attachment.depth_comparison_function);
    out.depth_bias = attachment.depth_bias;
    out.depth_bias_slope_scale = attachment.depth_bias_slope_scale;
    out.depth_bias_clamp = attachment.depth_bias_clamp;
    if (attachment.has_stencil) {
        api::depth_stencil_attachment::stencil_config sc;
        sc.should_store = attachment.stencil.should_store;
        if (attachment.stencil.has_clear_value) sc.clear_value = attachment.stencil.clear_value;
        sc.readonly = attachment.stencil.readonly;
        // ... (incomplete stencil config as noted in header)
        out.stencil = sc;
    }
    return out;
}

stylizer_render_pass_t stylizer_device_create_render_pass(stylizer_device_t device, const stylizer_color_attachment_t* colors, size_t color_count, const stylizer_depth_stencil_attachment_t* depth, bool one_shot, const char* label) {
    std::vector<api::color_attachment> cpp_colors;
    for (size_t i = 0; i < color_count; ++i) {
        cpp_colors.push_back(to_cpp(colors[i]));
    }
    std::optional<api::depth_stencil_attachment> cpp_depth;
    if (depth) {
        cpp_depth = to_cpp(*depth);
    }
    auto& rp = CAST_DEVICE(device)->create_render_pass(api::temporary_return, cpp_colors, cpp_depth, one_shot, label ? label : "Stylizer Render Pass");
    return reinterpret_cast<stylizer_render_pass_t>(rp.move_temporary_to_heap());
}

stylizer_compute_pipeline_t stylizer_device_create_compute_pipeline(stylizer_device_t device, const stylizer_pipeline_entry_point_t* entry_point, const char* label) {
    api::pipeline::entry_point cpp_ep;
    if (entry_point) {
        cpp_ep.shader = CAST_SHADER(entry_point->shader);
        if (entry_point->entry_point_name) cpp_ep.entry_point_name = entry_point->entry_point_name;
    }
    auto& cp = CAST_DEVICE(device)->create_compute_pipeline(api::temporary_return, cpp_ep, label ? label : "Stylizer Compute Pipeline");
    return reinterpret_cast<stylizer_compute_pipeline_t>(cp.move_temporary_to_heap());
}

stylizer_render_pipeline_t stylizer_device_create_render_pipeline(stylizer_device_t device, const stylizer_pipeline_entry_points_element_t* entry_points, size_t entry_point_count, const stylizer_color_attachment_t* color_attachments, size_t color_attachment_count, const stylizer_depth_stencil_attachment_t* depth_attachment, const stylizer_render_pipeline_config_t* config, const char* label) {
    api::pipeline::entry_points cpp_eps;
    for (size_t i = 0; i < entry_point_count; ++i) {
        api::pipeline::entry_point ep;
        ep.shader = CAST_SHADER(entry_points[i].entry_point.shader);
        if (entry_points[i].entry_point.entry_point_name) ep.entry_point_name = entry_points[i].entry_point.entry_point_name;
        cpp_eps[static_cast<api::shader_stage>(entry_points[i].stage)] = ep;
    }
    std::vector<api::color_attachment> cpp_colors;
    for (size_t i = 0; i < color_attachment_count; ++i) {
        cpp_colors.push_back(to_cpp(color_attachments[i]));
    }
    std::optional<api::depth_stencil_attachment> cpp_depth;
    if (depth_attachment) {
        cpp_depth = to_cpp(*depth_attachment);
    }
    api::render_pipeline::config cpp_config;
    if (config) {
        cpp_config.primitive_topology = static_cast<api::render_pipeline::config::primitive_topology>(config->primitive_topology);
        cpp_config.line_pixel_width = config->line_pixel_width;
        cpp_config.cull_mode = static_cast<api::render_pipeline::config::cull_mode>(config->cull_mode);
        cpp_config.winding_order_clockwise = config->winding_order_clockwise;
        cpp_config.u16_indices = config->u16_indices;
        for (size_t i = 0; i < config->vertex_buffer_count; ++i) {
            api::render_pipeline::config::vertex_buffer_layout vbl;
            vbl.per_instance = config->vertex_buffers[i].per_instance;
            if (config->vertex_buffers[i].has_stride_override) vbl.stride_override = config->vertex_buffers[i].stride_override;
            for (size_t j = 0; j < config->vertex_buffers[i].attribute_count; ++j) {
                api::render_pipeline::config::vertex_buffer_layout::attribute attr;
                attr.format = static_cast<api::render_pipeline::config::vertex_buffer_layout::attribute::format>(config->vertex_buffers[i].attributes[j].format);
                if (config->vertex_buffers[i].attributes[j].has_offset_override) attr.offset_override = config->vertex_buffers[i].attributes[j].offset_override;
                if (config->vertex_buffers[i].attributes[j].has_shader_location_override) attr.shader_location_override = config->vertex_buffers[i].attributes[j].shader_location_override;
                vbl.attributes.push_back(attr);
            }
            cpp_config.vertex_buffers.push_back(vbl);
        }
        cpp_config.multisampling.count = config->multisampling.count;
        cpp_config.multisampling.mask = config->multisampling.mask;
        cpp_config.multisampling.alpha_to_coverage = config->multisampling.alpha_to_coverage;
    }
    auto& rp = CAST_DEVICE(device)->create_render_pipeline(api::temporary_return, cpp_eps, cpp_colors, cpp_depth, cpp_config, label ? label : "Stylizer Render Pipeline");
    return reinterpret_cast<stylizer_render_pipeline_t>(rp.move_temporary_to_heap());
}

// Surface methods
void stylizer_surface_release(stylizer_surface_t surface) {
    if (surface) {
        auto s = CAST_SURFACE(surface);
        s->release();
        delete s;
    }
}
void stylizer_surface_configure(stylizer_surface_t surface, stylizer_device_t device, const stylizer_surface_config_t* config) {
    api::surface::config cpp_config;
    if (config) {
        cpp_config.present_mode = static_cast<api::surface::present_mode>(config->present_mode);
        cpp_config.texture_format = static_cast<api::texture_format>(config->texture_format);
        cpp_config.alpha_mode = static_cast<api::alpha_mode>(config->alpha_mode);
        cpp_config.usage = static_cast<api::usage>(config->usage);
        cpp_config.size = { config->size.x, config->size.y };
    }
    CAST_SURFACE(surface)->configure(*CAST_DEVICE(device), cpp_config);
}
stylizer_texture_t stylizer_surface_next_texture(stylizer_surface_t surface, stylizer_device_t device) {
    auto& tex = CAST_SURFACE(surface)->next_texture(api::temporary_return, *CAST_DEVICE(device));
    return reinterpret_cast<stylizer_texture_t>(tex.move_temporary_to_heap());
}
stylizer_texture_format_t stylizer_surface_configured_texture_format(stylizer_surface_t surface, stylizer_device_t device) {
    return static_cast<stylizer_texture_format_t>(CAST_SURFACE(surface)->configured_texture_format(*CAST_DEVICE(device)));
}
void stylizer_surface_present(stylizer_surface_t surface, stylizer_device_t device) {
    CAST_SURFACE(surface)->present(*CAST_DEVICE(device));
}

// Texture methods
void stylizer_texture_release(stylizer_texture_t texture) {
    if (texture) {
        auto t = CAST_TEXTURE(texture);
        t->release();
        delete t;
    }
}
stylizer_texture_view_t stylizer_texture_create_view(stylizer_texture_t texture, stylizer_device_t device, const stylizer_texture_view_create_config_t* config) {
    api::texture_view::create_config cpp_config;
    if (config) {
        cpp_config.base_mip_level = config->base_mip_level;
        if (config->has_mip_level_count_override) cpp_config.mip_level_count_override = config->mip_level_count_override;
        cpp_config.treat_as_cubemap = config->treat_as_cubemap;
        cpp_config.aspect = static_cast<api::texture_view::create_config::aspect>(config->aspect);
    }
    auto& view = CAST_TEXTURE(texture)->create_view(api::temporary_return, *CAST_DEVICE(device), cpp_config);
    return reinterpret_cast<stylizer_texture_view_t>(view.move_temporary_to_heap());
}
stylizer_vec3u_t stylizer_texture_get_size(stylizer_texture_t texture) {
    auto s = CAST_TEXTURE(texture)->size();
    return { s.x, s.y, s.z };
}
stylizer_texture_format_t stylizer_texture_get_format(stylizer_texture_t texture) {
    return static_cast<stylizer_texture_format_t>(CAST_TEXTURE(texture)->texture_format());
}
stylizer_usage_t stylizer_texture_get_usage(stylizer_texture_t texture) {
    return static_cast<stylizer_usage_t>(CAST_TEXTURE(texture)->usage());
}
void stylizer_texture_write(stylizer_texture_t texture, stylizer_device_t device, const void* data, size_t data_size, const stylizer_texture_data_layout_t* layout, stylizer_vec3u_t extent, const stylizer_vec3u_t* origin, size_t mip_level) {
    api::texture::data_layout cpp_layout = { layout->offset, layout->bytes_per_row, layout->rows_per_image };
    std::optional<api::vec3u> cpp_origin;
    if (origin) cpp_origin = { origin->x, origin->y, origin->z };
    CAST_TEXTURE(texture)->write(*CAST_DEVICE(device), std::span<const std::byte>(static_cast<const std::byte*>(data), data_size), cpp_layout, { extent.x, extent.y, extent.z }, cpp_origin, mip_level);
}
void stylizer_texture_copy_from(stylizer_texture_t texture, stylizer_device_t device, stylizer_texture_t source, const stylizer_vec3u_t* destination_origin, const stylizer_vec3u_t* source_origin, const stylizer_vec3u_t* extent_override, const size_t* min_mip_level, const size_t* mip_levels_override) {
    std::optional<api::vec3u> dest_o, src_o, extent;
    if (destination_origin) dest_o = { destination_origin->x, destination_origin->y, destination_origin->z };
    if (source_origin) src_o = { source_origin->x, source_origin->y, source_origin->z };
    if (extent_override) extent = { extent_override->x, extent_override->y, extent_override->z };
    std::optional<size_t> min_mip, mip_levels;
    if (min_mip_level) min_mip = *min_mip_level;
    if (mip_levels_override) mip_levels = *mip_levels_override;
    CAST_TEXTURE(texture)->copy_from(*CAST_DEVICE(device), *CAST_TEXTURE(source), dest_o, src_o, extent, min_mip, mip_levels);
}
void stylizer_texture_generate_mipmaps(stylizer_texture_t texture, stylizer_device_t device, const size_t* first_mip_level, const size_t* mip_levels_override) {
    std::optional<size_t> first, count;
    if (first_mip_level) first = *first_mip_level;
    if (mip_levels_override) count = *mip_levels_override;
    CAST_TEXTURE(texture)->generate_mipmaps(*CAST_DEVICE(device), first, count);
}

// TextureView methods
void stylizer_texture_view_release(stylizer_texture_view_t view) {
    if (view) {
        auto tv = CAST_TEXTURE_VIEW(view);
        tv->release();
        delete tv;
    }
}

// Buffer methods
void stylizer_buffer_release(stylizer_buffer_t buffer) {
    if (buffer) {
        auto b = CAST_BUFFER(buffer);
        b->release();
        delete b;
    }
}
void stylizer_buffer_write(stylizer_buffer_t buffer, stylizer_device_t device, const void* data, size_t data_size, size_t offset) {
    CAST_BUFFER(buffer)->write(*CAST_DEVICE(device), std::span<const std::byte>(static_cast<const std::byte*>(data), data_size), offset);
}
size_t stylizer_buffer_get_size(stylizer_buffer_t buffer) {
    return CAST_BUFFER(buffer)->size();
}
stylizer_usage_t stylizer_buffer_get_usage(stylizer_buffer_t buffer) {
    return static_cast<stylizer_usage_t>(CAST_BUFFER(buffer)->usage());
}
void* stylizer_buffer_map(stylizer_buffer_t buffer, stylizer_device_t device, bool for_writing, size_t offset, size_t size) {
    return CAST_BUFFER(buffer)->map(*CAST_DEVICE(device), for_writing, offset, size);
}
void stylizer_buffer_unmap(stylizer_buffer_t buffer) {
    CAST_BUFFER(buffer)->unmap();
}

// Shader methods
void stylizer_shader_release(stylizer_shader_t shader) {
    if (shader) {
        auto s = CAST_SHADER(shader);
        s->release();
        delete s;
    }
}

// BindGroup methods
void stylizer_bind_group_release(stylizer_bind_group_t bind_group) {
    if (bind_group) {
        auto bg = CAST_BIND_GROUP(bind_group);
        bg->release();
        delete bg;
    }
}

// Helpers for bind group bindings
static std::vector<api::bind_group::binding> to_cpp(const stylizer_bind_group_binding_t* bindings, size_t count) {
    std::vector<api::bind_group::binding> out;
    for (size_t i = 0; i < count; ++i) {
        if (bindings[i].type == STYLIZER_BINDING_TYPE_BUFFER) {
            api::bind_group::buffer_binding bb;
            bb.buffer = CAST_BUFFER(bindings[i].binding.buffer.buffer);
            bb.offset = bindings[i].binding.buffer.offset;
            if (bindings[i].binding.buffer.has_size_override) bb.size_override = bindings[i].binding.buffer.size_override;
            out.push_back(bb);
        } else {
            api::bind_group::texture_binding tb;
            tb.texture = CAST_TEXTURE(bindings[i].binding.texture.texture);
            tb.texture_view = CAST_TEXTURE_VIEW(bindings[i].binding.texture.texture_view);
            if (bindings[i].binding.texture.has_sampled_override) tb.sampled_override = bindings[i].binding.texture.sampled_override;
            out.push_back(tb);
        }
    }
    return out;
}

// ComputePipeline methods
void stylizer_compute_pipeline_release(stylizer_compute_pipeline_t pipeline) {
    if (pipeline) {
        auto cp = CAST_COMPUTE_PIPELINE(pipeline);
        cp->release();
        delete cp;
    }
}
stylizer_bind_group_t stylizer_compute_pipeline_create_bind_group(stylizer_compute_pipeline_t pipeline, stylizer_device_t device, size_t index, const stylizer_bind_group_binding_t* bindings, size_t binding_count, const char* label) {
    auto cpp_bindings = to_cpp(bindings, binding_count);
    auto& bg = CAST_COMPUTE_PIPELINE(pipeline)->create_bind_group(api::temporary_return, *CAST_DEVICE(device), index, cpp_bindings, label ? label : "Stylizer Bind Group");
    return reinterpret_cast<stylizer_bind_group_t>(bg.move_temporary_to_heap());
}

// RenderPipeline methods
void stylizer_render_pipeline_release(stylizer_render_pipeline_t pipeline) {
    if (pipeline) {
        auto rp = CAST_RENDER_PIPELINE(pipeline);
        rp->release();
        delete rp;
    }
}
stylizer_bind_group_t stylizer_render_pipeline_create_bind_group(stylizer_render_pipeline_t pipeline, stylizer_device_t device, size_t index, const stylizer_bind_group_binding_t* bindings, size_t binding_count, const char* label) {
    auto cpp_bindings = to_cpp(bindings, binding_count);
    auto& bg = CAST_RENDER_PIPELINE(pipeline)->create_bind_group(api::temporary_return, *CAST_DEVICE(device), index, cpp_bindings, label ? label : "Stylizer Bind Group");
    return reinterpret_cast<stylizer_bind_group_t>(bg.move_temporary_to_heap());
}

// CommandEncoder methods
void stylizer_command_encoder_release(stylizer_command_encoder_t encoder) {
    if (encoder) {
        auto ce = CAST_COMMAND_ENCODER(encoder);
        ce->release();
        delete ce;
    }
}
void stylizer_command_encoder_copy_buffer_to_buffer(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_buffer_t destination, stylizer_buffer_t source, const size_t* destination_offset, const size_t* source_offset, const size_t* size_override) {
    std::optional<size_t> dest_o, src_o, size;
    if (destination_offset) dest_o = *destination_offset;
    if (source_offset) src_o = *source_offset;
    if (size_override) size = *size_override;
    CAST_COMMAND_ENCODER(encoder)->copy_buffer_to_buffer(*CAST_DEVICE(device), *CAST_BUFFER(destination), *CAST_BUFFER(source), dest_o, src_o, size);
}
void stylizer_command_encoder_bind_compute_pipeline(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_compute_pipeline_t pipeline, bool release_on_submit) {
    CAST_COMMAND_ENCODER(encoder)->bind_compute_pipeline(*CAST_DEVICE(device), *CAST_COMPUTE_PIPELINE(pipeline), release_on_submit);
}
void stylizer_command_encoder_bind_compute_group(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_bind_group_t group, const bool* release_on_submit, const size_t* index_override) {
    std::optional<bool> release;
    if (release_on_submit) release = *release_on_submit;
    std::optional<size_t> index;
    if (index_override) index = *index_override;
    CAST_COMMAND_ENCODER(encoder)->bind_compute_group(*CAST_DEVICE(device), *CAST_BIND_GROUP(group), release, index);
}
void stylizer_command_encoder_dispatch_workgroups(stylizer_command_encoder_t encoder, stylizer_device_t device, stylizer_vec3u_t workgroups) {
    CAST_COMMAND_ENCODER(encoder)->dispatch_workgroups(*CAST_DEVICE(device), { workgroups.x, workgroups.y, workgroups.z });
}
stylizer_command_buffer_t stylizer_command_encoder_end(stylizer_command_encoder_t encoder, stylizer_device_t device) {
    auto& cb = CAST_COMMAND_ENCODER(encoder)->end(api::temporary_return, *CAST_DEVICE(device));
    return reinterpret_cast<stylizer_command_buffer_t>(cb.move_temporary_to_heap());
}
void stylizer_command_encoder_one_shot_submit(stylizer_command_encoder_t encoder, stylizer_device_t device) {
    CAST_COMMAND_ENCODER(encoder)->one_shot_submit(*CAST_DEVICE(device));
}

// RenderPass methods
void stylizer_render_pass_release(stylizer_render_pass_t pass) {
    if (pass) {
        auto rp = CAST_RENDER_PASS(pass);
        rp->release();
        delete rp;
    }
}
void stylizer_render_pass_bind_render_pipeline(stylizer_render_pass_t pass, stylizer_device_t device, stylizer_render_pipeline_t pipeline, bool release_on_submit) {
    CAST_RENDER_PASS(pass)->bind_render_pipeline(*CAST_DEVICE(device), *CAST_RENDER_PIPELINE(pipeline), release_on_submit);
}
void stylizer_render_pass_bind_render_group(stylizer_render_pass_t pass, stylizer_device_t device, stylizer_bind_group_t group, const bool* release_on_submit, const size_t* index_override) {
    std::optional<bool> release;
    if (release_on_submit) release = *release_on_submit;
    std::optional<size_t> index;
    if (index_override) index = *index_override;
    CAST_RENDER_PASS(pass)->bind_render_group(*CAST_DEVICE(device), *CAST_BIND_GROUP(group), release, index);
}
void stylizer_render_pass_bind_vertex_buffer(stylizer_render_pass_t pass, stylizer_device_t device, size_t slot, stylizer_buffer_t buffer, const size_t* offset, const size_t* size_override) {
    std::optional<size_t> off, size;
    if (offset) off = *offset;
    if (size_override) size = *size_override;
    CAST_RENDER_PASS(pass)->bind_vertex_buffer(*CAST_DEVICE(device), slot, *CAST_BUFFER(buffer), off, size);
}
void stylizer_render_pass_bind_index_buffer(stylizer_render_pass_t pass, stylizer_device_t device, stylizer_buffer_t buffer, const size_t* offset, const size_t* size_override) {
    std::optional<size_t> off, size;
    if (offset) off = *offset;
    if (size_override) size = *size_override;
    CAST_RENDER_PASS(pass)->bind_index_buffer(*CAST_DEVICE(device), *CAST_BUFFER(buffer), off, size);
}
void stylizer_render_pass_draw(stylizer_render_pass_t pass, stylizer_device_t device, size_t vertex_count, const size_t* instance_count, const size_t* first_vertex, size_t first_instance) {
    std::optional<size_t> inst, first;
    if (instance_count) inst = *instance_count;
    if (first_vertex) first = *first_vertex;
    CAST_RENDER_PASS(pass)->draw(*CAST_DEVICE(device), vertex_count, inst, first, first_instance);
}
void stylizer_render_pass_draw_indexed(stylizer_render_pass_t pass, stylizer_device_t device, size_t index_count, const size_t* instance_count, const size_t* first_index, const size_t* base_vertex, size_t first_instance) {
    std::optional<size_t> inst, first, base;
    if (instance_count) inst = *instance_count;
    if (first_index) first = *first_index;
    if (base_vertex) base = *base_vertex;
    CAST_RENDER_PASS(pass)->draw_indexed(*CAST_DEVICE(device), index_count, inst, first, base, first_instance);
}
stylizer_command_buffer_t stylizer_render_pass_end(stylizer_render_pass_t pass, stylizer_device_t device) {
    auto& cb = CAST_RENDER_PASS(pass)->end(api::temporary_return, *CAST_DEVICE(device));
    return reinterpret_cast<stylizer_command_buffer_t>(cb.move_temporary_to_heap());
}

// CommandBuffer methods
void stylizer_command_buffer_release(stylizer_command_buffer_t buffer) {
    if (buffer) {
        auto cb = CAST_COMMAND_BUFFER(buffer);
        cb->release();
        delete cb;
    }
}
void stylizer_command_buffer_submit(stylizer_command_buffer_t buffer, stylizer_device_t device, bool release) {
    CAST_COMMAND_BUFFER(buffer)->submit(*CAST_DEVICE(device), release);
}

} // extern "C"
