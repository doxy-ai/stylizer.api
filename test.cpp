#include "api.hpp"

#include <stub/stub.hpp>

#include <GLFW/glfw3.h>
#include <iostream>

int main() {
	auto& errors = stylizer::get_error_handler();
	stylizer::auto_release c = errors.connect([](stylizer::api::error::severity severity, std::string_view message, size_t) {
		if (severity > stylizer::api::error::severity::Error)
			throw stylizer::api::error(message);
		std::cerr << message << std::endl;
	});

	errors(stylizer::api::error::severity::Info, "Hello World", 0);

	if (!glfwInit()) {
		errors(stylizer::api::error::severity::Error, "Failed to initialize GLFW", 0);
		return -1;
	}
	glfwSetErrorCallback(+[](int type, const char* message) {
		stylizer::get_error_handler()(stylizer::api::error::severity::Error, std::string { "GLFW Error: " } + message, type);
	});
	defer_ { glfwTerminate(); };

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Stylizer Test", nullptr, nullptr);
	if (!window) {
		errors(stylizer::api::error::severity::Error, "Failed to create GLFW window", 0);
		return -1;
	}
	defer_ { glfwDestroyWindow(window); };

	glfwMakeContextCurrent(window);

	auto glClearColor = (void (*)(float, float, float, float))glfwGetProcAddress("glClearColor");
	if (!glClearColor) {
		errors(stylizer::api::error::severity::Error, "Failed to find glClearColor", 0);
		return -1;
	}

	auto glClear = (void (*)(uint32_t))glfwGetProcAddress("glClear");
	if (!glClear) {
		errors(stylizer::api::error::severity::Error, "Failed to find glClear", 0);
		return -1;
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(2.f/255, 7.f/255, 53.f/255, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
