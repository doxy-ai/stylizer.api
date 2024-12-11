#define STYLIZER_API_IMPLEMENTATION
#include "webgpu/all.hpp"
#include "glfw.hpp"

#include <iostream>

int main() {
	using namespace stylizer::api::operators;
	glfwInit();

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Stylizer Test", NULL, NULL);

	stylizer::auto_release surface = stylizer::api::glfw::create_surface<stylizer::api::webgpu::surface>(window);
	stylizer::auto_release device = stylizer::api::webgpu::device::create_default({.high_performance = true, .compatible_surface = &surface});

	auto config = surface.determine_optimal_config(device, stylizer::api::glfw::get_window_size(window));
	config.usage |= stylizer::api::usage::CopySource;
	surface.configure(device, config);

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

	std::vector<stylizer::api::color_attachment> color_attachments = {{
		.texture_format = stylizer::api::texture::format::BGRA8_SRGB,
		.clear_value = {{.3, .5, 1}},
	}};
	stylizer::auto_release pipeline = stylizer::api::webgpu::render_pipeline::create(
		device,
		{
			{stylizer::api::shader::stage::Vertex, {&shader, "vertex"}},
			{stylizer::api::shader::stage::Fragment, {&shader, "fragment"}}
		},
		color_attachments
	);

	{
		std::array<float, 5> data = {1, 2, 3, 4, 5};
		stylizer::auto_release buffer = device.create_and_write_buffer(stylizer::api::usage::Storage | stylizer::api::usage::CopySource, byte_span<float>(data));
		stylizer::auto_release shader = stylizer::api::webgpu::shader::create_from_source(device, stylizer::api::webgpu::shader::language::WGSL, R"(
@group(0) @binding(0) var<storage, read_write> data: array<f32>;

@compute @workgroup_size(1) fn compute(@builtin(global_invocation_id) id: vec3<u32>) {
	let i = id.x;
	data[i] = data[i] * data[i];
})");

		// stylizer::auto_release pipeline = device.create_compute_pipeline({&shader, "compute"});
		// device.create_command_encoder(true)
		// 	.bind_compute_pipeline(device, pipeline)
		// 	.bind_compute_group(device, pipeline.create_bind_group(device, 0, std::vector<stylizer::api::bind_group::binding>{
		// 		stylizer::api::bind_group::buffer_binding{&buffer}
		// 	}))
		// 	.dispatch_workgroups(device, {5, 1, 1})
		// 	.one_shot_submit(device);
		device.quick_compute_dispatch({5, 1, 1}, {&shader, "compute"}, std::vector<stylizer::api::bind_group::binding>{
			stylizer::api::bind_group::buffer_binding{&buffer}
		});

		stylizer::auto_release download_buffer = device.create_buffer(stylizer::api::usage::MapRead | stylizer::api::usage::CopyDestination, byte_span<float>(data).size());
		download_buffer.copy_from(device, buffer);
		std::span<float> dbg = {(float*)download_buffer.map(device), 5};
	}


	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		device.process_events();

		try {
			stylizer::auto_release texture = surface.next_texture(device);
			color_attachments[0].texture = &texture;
			device.create_render_pass(color_attachments, {}, true)
				.bind_render_pipeline(pipeline)
				.draw(3)
				.one_shot_submit(device);
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