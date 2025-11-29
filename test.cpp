#include "sdl3.hpp"
#include <backends/current_backend.hpp>

#include <GL/gl.h>
#include <iostream>

int main() {
	using namespace stylizer::api::operators;

	auto& errors = stylizer::get_error_handler();
	stylizer::auto_release c = errors.connect([](stylizer::api::error::severity severity, std::string_view message, size_t) {
		if (severity >= stylizer::api::error::severity::Error)
			throw stylizer::api::error(message);
		std::cerr << message << std::endl;
	});

	errors(stylizer::api::error::severity::Info, "Hello World", 0);

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		errors(stylizer::api::error::severity::Error, "Failed to initialize SDL", 0);
		return -1;
	}
	defer_ { SDL_Quit(); };

	stylizer::api::vec2u size = {800, 600};
	SDL_Window* window = SDL_CreateWindow("Stylizer::API Test", size.x, size.y, 0);
	if (!window) {
		errors(stylizer::api::error::severity::Error, "Failed to create SDL window", 0);
		return -1;
	}
	defer_ { SDL_DestroyWindow(window); };

	stylizer::auto_release<stylizer::api::current_backend::device> device;
	stylizer::auto_release<stylizer::api::current_backend::surface> surface;
	std::tie(device, surface) = stylizer::api::sdl3::create_surface_and_device<stylizer::api::current_backend::device, stylizer::api::current_backend::surface>(window);

	surface.configure(device, surface.determine_optimal_default_config(device, {800, 600}));

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
		stylizer::api::shader::language::Slang,
		stylizer::api::shader::stage::Vertex,
		source, "vertex"
	);
	stylizer::auto_release fragment_shader = device.create_shader_from_source(
		stylizer::api::shader::language::Slang,
		stylizer::api::shader::stage::Fragment,
		source, "fragment"
	);

	std::vector<stylizer::api::color_attachment> color_attachments = {{
		.texture_format = surface.configured_texture_format(device),
		.clear_value = {{2.f/255, 7.f/255, 53.f/255, 1.f}},
	}};
	stylizer::auto_release pipeline = device.create_render_pipeline(
		{
			{stylizer::api::shader::stage::Vertex, {&vertex_shader, "vertex"}},
			{stylizer::api::shader::stage::Fragment, {&fragment_shader, "fragment"}}
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
		} catch(stylizer::api::surface::texture_acquisition_failed fail) {
			std::cerr << fail.what() << std::endl;
		}
	}
}
