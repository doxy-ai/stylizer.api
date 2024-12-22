#define STYLIZER_API_IMPLEMENTATION
#include "webgpu/all.hpp"
#include "glfw.hpp"

#include <iostream>

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

	stylizer::auto_release shader = stylizer::api::webgpu::shader::create_from_source(device, stylizer::api::shader::language::WGSL, R"(
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

	// stylizer::auto_release texture = surface.next_texture(device);
	// stylizer::auto_release reference_render_pass = device.create_render_pass(
	// 	span_from_value<stylizer::api::color_attachment>({
	// 		.texture = &texture,
	// 		.clear_value = {{.3, .5, 1}},
	// 	})
	// );

	// stylizer::auto_release pipeline = stylizer::api::webgpu::graphics_pipeline::create_from_compatible_render_pass(device, {
	// 	{stylizer::api::shader::stage::Vertex, {&shader, "vertex"}},
	// 	{stylizer::api::shader::stage::Fragment, {&shader, "fragment"}}
	// }, reference_render_pass);

	// surface.present(device); // "Releases" the surface to fetch textures ("lock" grabbed for reference_render_pass)

	std::vector<stylizer::api::color_attachment> color_attachments = {{
		.texture_format = stylizer::api::texture::format::BGRA8_SRGB,
		.clear_value = {{.3, .5, 1}},
	}};
	stylizer::auto_release pipeline = stylizer::api::webgpu::graphics_pipeline::create(
		device,
		{
			{stylizer::api::shader::stage::Vertex, {&shader, "vertex"}},
			{stylizer::api::shader::stage::Fragment, {&shader, "fragment"}}
		},
		color_attachments//,
		// {},
		// {.vertex_buffers = {
		// 	{.attributes = {
		// 		{.format = stylizer::api::graphics_pipeline::config::vertex_buffer_layout::attribute::format_of<float>()}
		// 	}}
		// }}
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
		} catch(stylizer::api::surface::texture_acquisition_failed fail) {
			std::cerr << fail.what() << std::endl;
		}
	}

	device.release(true);
	surface.release();
	glfwDestroyWindow(window);
	glfwTerminate();
}