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
#include "util/defer.hpp"

#include <SDL3/SDL.h>

import std.compat;
import magic_enum;
import stylizer.errors;
import stylizer.auto_release;
import stylizer.spans;
import stylizer.graphics;
import stylizer.graphics.sdl3;
import stylizer.graphics.current_backend;

int main() {
    using namespace magic_enum::bitwise_operators;

    auto& errors = stylizer::get_error_handler();
    stylizer::auto_release c = errors.connect([](stylizer::error::severity severity, std::string_view message, size_t) {
        if (severity >= stylizer::error::severity::Error)
            throw stylizer::error(message);
        std::cerr << message << std::endl;
    });

    errors(stylizer::error::severity::Info, "Hello World", 0);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        errors(stylizer::error::severity::Error, "Failed to initialize SDL", 0);
        return -1;
    }
    defer_ { SDL_Quit(); };

    stylizer::graphics::vec2u size = {800, 600};
    SDL_Window* window = SDL_CreateWindow("Stylizer::Graphics Test", size.x, size.y, 0);
    if (!window) {
        errors(stylizer::error::severity::Error, "Failed to create SDL window", 0);
        return -1;
    }
    defer_ { SDL_DestroyWindow(window); };

    stylizer::auto_release<stylizer::graphics::current_backend::device> device;
    stylizer::auto_release<stylizer::graphics::current_backend::surface> surface;
    std::tie(device, surface) = stylizer::graphics::sdl3::create_surface_and_device<stylizer::graphics::current_backend::device, stylizer::graphics::current_backend::surface>(window);
    if(!device) {
        errors(stylizer::error::severity::Error, "Failed to create stylizer device", 0);
        return -1;
    }
    if(!surface) {
        errors(stylizer::error::severity::Error, "Failed to create stylizer surface", 0);
        return -1;
    }

    surface.configure(device, surface.determine_optimal_default_config(device, size));

    auto source = R"(
[shader("vertex")]
float4 vertex(uint index : SV_VertexID) : SV_Position {
    float2 p = float2(0.0, 0.0);
    if (index == 0) {
        p = float2(-0.5, -0.5);
    } else if (index == 1) {
        p = float2(0.5, -0.5);
    } else {
        p = float2(0.0, 0.5);
    }
    return float4(p, 0.0, 1.0);
}

[shader("fragment")]
float4 fragment(float4 : SV_Position) : SV_Target {
    return float4(231.0/255, 39.0/255, 37.0/255, 1.0);
})";

    stylizer::auto_release vertex_shader = device.create_shader_from_source(
        stylizer::graphics::shader::language::Slang,
        stylizer::graphics::shader::stage::Vertex,
        source, "vertex"
    );
    stylizer::auto_release fragment_shader = device.create_shader_from_source(
        stylizer::graphics::shader::language::Slang,
        stylizer::graphics::shader::stage::Fragment,
        source, "fragment"
    );

    std::vector<stylizer::graphics::color_attachment> color_attachments = {{
        .texture_format = surface.configured_texture_format(device),
        .clear_value = {{2.f/255, 7.f/255, 53.f/255, 1.f}},
    }};
    stylizer::auto_release pipeline = device.create_render_pipeline(
        {
            {stylizer::graphics::shader::stage::Vertex, {&vertex_shader, "vertex"}},
            {stylizer::graphics::shader::stage::Fragment, {&fragment_shader, "fragment"}}
        },
        color_attachments
    );

    bool should_close = false;
    while (!should_close) {
        for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type) {
        break; case SDL_EVENT_QUIT:
            should_close = true;
        }
        device.process_events();

        try {
            stylizer::auto_release texture = surface.next_texture(device);
            color_attachments[0].texture = &texture;
            device.create_render_pass(color_attachments, {}, true)
                .bind_render_pipeline(device, pipeline)
                .draw(device, 3)
                .one_shot_submit(device);
            surface.present(device);
        } catch(stylizer::graphics::surface::texture_acquisition_failed fail) {
            std::cerr << fail.what() << std::endl;
        }
    }

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
