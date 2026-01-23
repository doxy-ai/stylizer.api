#include "util/defer.hpp"

#include <SDL3/SDL.h>
#include <GL/gl.h>

#include <cassert>

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

	std::vector<int> data = {1, 2, 3, 4};
	stylizer::auto_release buffer = device.create_and_write_buffer(stylizer::graphics::usage::CopySource | stylizer::graphics::usage::Storage, stylizer::byte_span<int>(data));

	stylizer::auto_release compute_shader = device.create_shader_from_source(stylizer::graphics::shader::language::Slang, stylizer::graphics::shader::stage::Compute, R"__(
RWStructuredBuffer<int> buffer;

[shader("compute"), numthreads(5, 1, 1)]
void compute(vector<int, 3> id : SV_DispatchThreadID) {
	int i = id.x;
	buffer[i] = buffer[i] * 5;
}
	)__", "compute");

	stylizer::auto_release pipeline = device.create_compute_pipeline({&compute_shader, "compute"});
	stylizer::auto_release result_buffer = device.create_and_write_buffer(stylizer::graphics::usage::MapRead, stylizer::byte_span<int>(data));
	device.create_command_encoder(true)
		.bind_compute_pipeline(device, pipeline)
		.bind_compute_group(device, pipeline.create_bind_group(device, 0, stylizer::span_from_value<stylizer::graphics::bind_group::binding>(stylizer::graphics::bind_group::buffer_binding{&buffer})))
		.dispatch_workgroups(device, {1, 1, 1})
		.one_shot_submit(device);

	result_buffer.copy_from(device, buffer);
	auto ints = (int*)result_buffer.map(device);
	std::cout << ints << std::endl;
	assert(ints[0] == 5);
	assert(ints[1] == 10);
	assert(ints[2] == 15);
	assert(ints[3] == 20);
	result_buffer.unmap();

	errors(stylizer::error::severity::Info, "Computation Finished", 0);

	bool should_close = false;
	while (!should_close) {
		for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type) {
		break; case SDL_EVENT_QUIT:
			should_close = true;
		}

		stylizer::auto_release texture = surface.next_texture(device);

		surface.present(device);
	}
}
