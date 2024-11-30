#define STYLIZER_API_IMPLEMENTATION
#include "webgpu/device.hpp"
#include "webgpu/surface.hpp"
#include "glfw.hpp"

#include <iostream>

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Stylizer Test", NULL, NULL);

	stylizer::auto_release surface = stylizer::api::create_surface_from_glfw<stylizer::api::webgpu::surface>(window);
	auto device = stylizer::api::webgpu::device::create_default({.high_performance = true, .compatible_surface = &surface});

	surface.configure(device, surface.determine_optimal_config(device, stylizer::api::get_glfw_window_size(window)));

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// surface.present(device);
	}

	device.release(true);
	surface.release();
	glfwDestroyWindow(window);
	glfwTerminate();
}