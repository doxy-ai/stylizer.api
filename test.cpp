#include "sdl3.hpp"
#include <backends/current_backend.hpp>

#include <GL/gl.h>
#include <iostream>

int main() {
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
	if(!device) {
		errors(stylizer::api::error::severity::Error, "Failed to create stylizer device", 0);
		return -1;
	}
	if(!surface) {
		errors(stylizer::api::error::severity::Error, "Failed to create stylizer surface", 0);
		return -1;
	}

	surface.configure(device, surface.determine_optimal_default_config(device, size));

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
