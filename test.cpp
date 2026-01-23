#include "util/defer.hpp"

#include <SDL3/SDL.h>
#include <GL/gl.h>

import std.compat;
import stylizer.errors;
import stylizer.auto_release;
import stylizer.graphics;
import stylizer.graphics.sdl3;
import stylizer.graphics.current_backend;

int main() {
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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("Stylizer Test", 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		errors(stylizer::error::severity::Error, "Failed to create SDL window", 0);
		return -1;
	}
	defer_ { SDL_DestroyWindow(window); };

	stylizer::auto_release device = stylizer::graphics::current_backend::device::create_default();

	SDL_GL_CreateContext(window);

	auto glClearColor = (void (*)(float, float, float, float))SDL_GL_GetProcAddress("glClearColor");
	if (!glClearColor) {
		errors(stylizer::error::severity::Error, "Failed to find glClearColor", 0);
		return -1;
	}

	auto glClear = (void (*)(uint32_t))SDL_GL_GetProcAddress("glClear");
	if (!glClear) {
		errors(stylizer::error::severity::Error, "Failed to find glClear", 0);
		return -1;
	}

	bool should_close = false;
	while (!should_close) {
		for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type) {
		break; case SDL_EVENT_QUIT:
			should_close = true;
		}

		glClearColor(2.f/255, 7.f/255, 53.f/255, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}
}
