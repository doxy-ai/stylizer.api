#define STYLIZER_API_IMPLEMENTATION
#include "webgpu/device.hpp"
#include "webgpu/surface.hpp"
#include "webgpu/render_pass.hpp"
#include "glfw.hpp"

#include <iostream>
#include "span_from_value.hpp"

int main() {
	glfwInit();

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Stylizer Test", NULL, NULL);

	stylizer::auto_release surface = stylizer::api::create_surface_from_glfw<stylizer::api::webgpu::surface>(window);
	auto device = stylizer::api::webgpu::device::create_default({.high_performance = true, .compatible_surface = &surface});

	surface.configure(device, surface.determine_optimal_config(device, stylizer::api::get_glfw_window_size(window)));

	std::array<stylizer::api::color8, 640 * 480> color_data; color_data.fill({1, 1, 1, 1});
	constexpr size_t color8size = sizeof(stylizer::api::color8);

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		device.process_events();

		try {
			stylizer::auto_release texture = surface.next_texture(device);
			stylizer::auto_release render_pass = device.create_render_pass(
				span_from_value<stylizer::api::render_pass::color_attachment>({
					.texture = texture,
					.clear_value = {{.3, .5, 1}},
				})
			);

			render_pass.submit(device);
			surface.present(device);
		} catch(stylizer::api::surface::texture_aquisition_failed fail) {
			std::cerr << fail.what() << std::endl;
		}
	}

	device.release(true);
	surface.release();
	glfwDestroyWindow(window);
	glfwTerminate();
}