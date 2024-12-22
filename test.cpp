#define STYLIZER_API_IMPLEMENTATION
#include "current_platform.hpp"
#include "glfw.hpp"

#include <iostream>

int main() {
	using namespace stylizer::api::current_platform;

	glfwInit();

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Stylizer Test", NULL, NULL);

	stylizer::auto_release surface = stylizer::api::glfw::create_surface<struct surface>(window);
	stylizer::auto_release device = device::create_default({.high_performance = true, .compatible_surface = &surface});

	surface.configure(device, surface.determine_optimal_config(device, stylizer::api::glfw::get_window_size(window)));

	std::array<stylizer::api::color8, 640 * 480> color_data; color_data.fill({1, 1, 1, 1});
	constexpr size_t color8size = sizeof(stylizer::api::color8);

	stylizer::auto_release shader = shader::create_from_source(device, shader::language::WGSL, R"(
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

	std::vector<render_pass::color_attachment> color_attachments = {{
		.texture_format = texture::format::BGRA8_SRGB,
		.clear_value = {{.3, .5, 1}},
	}};
	stylizer::auto_release pipeline = graphics_pipeline::create(
		device,
		{
			{shader::stage::Vertex, {&shader, "vertex"}},
			{shader::stage::Fragment, {&shader, "fragment"}}
		},
		color_attachments
	);

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		device.process_events();

		try {
			stylizer::auto_release texture = surface.next_texture(device);
			color_attachments[0].texture = &texture;
			stylizer::auto_release render_pass = device.create_render_pass(color_attachments);
			render_pass.bind_pipeline(pipeline);
			render_pass.draw(3);

			render_pass.submit(device);
			surface.present(device);
		} catch(surface::texture_acquisition_failed fail) {
			std::cerr << fail.what() << std::endl;
		}
	}

	device.release(true);
	surface.release();
	glfwDestroyWindow(window);
	glfwTerminate();
}