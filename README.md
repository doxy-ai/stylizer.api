# Stylizer::API

**Stylizer::API** is a modern C++ graphics API.
It provides a clean, RAII-friendly API for managing GPU resources, rendering pipelines, shaders, surfaces, and error handling.

The library is designed to minimize boilerplate and maximize safety by leveraging C++ idioms such as smart handle like wrappers (`auto_release`) and unified error reporting.

---

## Features

- **Error Handling**
Centralized, severity-aware error reporting system with user-defined handlers.

- **RAII Resource Management**
Automatic cleanup of GPU objects, surfaces, and other resources via `auto_release`.

- **SDL3 and GLFW Integration**
Easily create rendering surfaces from `SDL_Window` or `GLFWwindow*`.

- **Works on Major Desktop Platforms** (WiP)
  - Windows (theoretical)
  - Linux (tested)
  - Mac (theoretical)

- **Multiple Supported Backends** (WiP)
  - WebGPU (Vulkan, DirectX, Metal, OpenGL)  

- **Shader Management** (WiP)
Compile shaders from multiple common languages:
  - **SPIR-V**
  - **WGSL**
  - **Slang**

---

## Build Instructions

1. Add **Stylizer.API** to your cmake project:

   ```cmake
   set(CMAKE_CXX_STANDARD 26)
   
   add_subdirectory(stylizer.api)
   target_link_libraries(app PUBLIC stylizer::api::webgpu) # Swap webgpu with whichever backend you prefer
   ```

2. Optionally install extra dependencies:
   - Windowing library ([SDL3](https://github.com/libsdl-org/SDL), [GLFW](https://www.glfw.org/), and most native platform windows are supported.)

---

## Example Usage

Below is a minimal example showing how to initialize SDL, set up a WebGPU surface, compile a WGSL shader, and render a triangle:

```cpp
#include "sdl3.hpp"
#include <GL/gl.h>
#include <iostream>
#include <webgpu/webgpu.hpp>

int main() {
    using namespace stylizer::api::operators;

    // Error handler setup
    auto& errors = stylizer::get_error_handler();
    stylizer::auto_release c = errors.connect([](stylizer::api::error::severity severity, std::string_view message, size_t) {
        if (severity >= stylizer::api::error::severity::Error)
            throw stylizer::api::error(message);
        std::cerr << message << std::endl;
    });

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        errors(stylizer::api::error::severity::Error, "Failed to initialize SDL", 0);
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Stylizer.API Test", 800, 600, 0);
    if (!window) {
        errors(stylizer::api::error::severity::Error, "Failed to create SDL window", 0);
        return -1;
    }

    // Create surface + device
    stylizer::auto_release<stylizer::api::current_backend::device> device;
    stylizer::auto_release<stylizer::api::current_backend::surface> surface;
    std::tie(device, surface) = stylizer::api::sdl3::create_surface_and_device<stylizer::api::current_backend::device, stylizer::api::current_backend::surface>(window);

    surface.configure(device, surface.determine_optimal_default_config(device, {800, 600}));

    // Compile WGSL shader
    stylizer::auto_release shader = device.create_shader_from_source(
        stylizer::api::shader::language::WGSL,
        R"(
@vertex
fn vertex(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
    var p = vec2f(0.0, 0.0);
    if (in_vertex_index == 0u) {
        p = vec2f(-0.5, -0.5);
    } else if (in_vertex_index == 1u) {
        p = vec2f(0.5, -0.5);
    } else {
        p = vec2f(0.0, 0.5);
    }
    return vec4f(p, 0.0, 1.0);
}

@fragment
fn fragment() -> @location(0) vec4f {
    return vec4f(1.0, 0.4, 1.0, 1.0);
})");

    // Render pipeline
    std::vector<stylizer::api::color_attachment> color_attachments = {{
        .texture_format = surface.configured_texture_format(device),
        .clear_value = {{.3f, .5f, 1}},
    }};
    stylizer::auto_release pipeline = device.create_render_pipeline(
        {
            {stylizer::api::shader::stage::Vertex, {&shader, "vertex"}},
            {stylizer::api::shader::stage::Fragment, {&shader, "fragment"}}
        },
        color_attachments
    );

    // Main loop
    bool should_close = false;
    while (!should_close) {
        for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type) {
        break; case SDL_EVENT_QUIT:
            should_close = true;
        }
        device.process_events();

        try {
            auto texture = std::unique_ptr<stylizer::api::texture>(
                surface.next_texture(stylizer::api::temporary_return, device).move_temporary_to_heap()
            );
            color_attachments[0].texture = texture.get();
            device.create_render_pass(color_attachments, {}, true)
                .bind_render_pipeline(device, pipeline)
                .draw(device, 3)
                .one_shot_submit(device);
            surface.present(device);
        } catch(stylizer::api::surface::texture_acquisition_failed fail) {
            std::cerr << fail.what() << std::endl;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
```

---

## Dependencies

- A modern C++26 compiler

### Included Dependencies

- [magic_enum](https://github.com/Neargye/magic_enum)
- [std::math](https://github.com/doxy-ai/stdmath)
- [signals](https://github.com/TheWisp/signals)
- [SLCross](https://github.com/doxy-ai/slcross) (Slang, Tint, GLSLang)
- [Dawn](https://dawn.googlesource.com/dawn) (if building for WebGPU)

---

## License

MIT License. See [LICENSE](./LICENSE) for details.
