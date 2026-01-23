export module stylizer.graphics.webgpu;

export import :bind_group;
export import :buffer;
export import :command_buffer;
export import :command_encoder;
export import :compute_pipeline;
export import :render_pass;
export import :device;
export import :render_pipeline;
export import :shader;
export import :surface;
export import :texture_view;
export import :texture;

static_assert(stylizer::graphics::device_concept<stylizer::graphics::webgpu::device>);