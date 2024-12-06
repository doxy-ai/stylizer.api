#include "device.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "render_pass.hpp"

namespace stylizer::api::webgpu {
	device device::create_default(const create_config& config) {
		device out;
		out.adapter = get_instance().requestAdapter(WGPURequestAdapterOptions{
			.compatibleSurface = confirm_wgpu_type<webgpu::surface>(*config.surface).surface_,
			.powerPreference = config.high_performance ? wgpu::PowerPreference::HighPerformance : wgpu::PowerPreference::LowPower,
		});
		out.device_ = out.adapter.requestDevice({});
		out.queue = out.device_.getQueue();
		return out;
	}

	webgpu::texture device::create_texture(const api::texture::create_config& config /* = {} */) {
		return webgpu::texture::create(*this, config);
	}
	api::texture& device::create_texture(temporary_return_t, const api::texture::create_config& config /* = {} */) {
		static webgpu::texture texture;
		return texture = create_texture(config);
	}

	webgpu::buffer device::create_buffer(usage usage, size_t size, bool mapped_at_creation /* = false */, std::string_view label /* = "Stylizer Buffer" */) {
		return webgpu::buffer::create(*this, usage, size, mapped_at_creation, label);
	}
	api::buffer& device::create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation/*  = false */, std::string_view label /* = "Stylizer Buffer" */) {
		static webgpu::buffer buffer;
		return buffer = create_buffer(usage, size, mapped_at_creation, label);
	}
	webgpu::buffer device::create_and_write_buffer(usage usage, std::span<std::byte> data, size_t offset /* = 0 */, std::string_view label /* = "Stylizer Buffer" */) {
		return webgpu::buffer::create_and_write(*this, usage, data, offset, label);
	}
	api::buffer& device::create_and_write_buffer(temporary_return_t, usage usage, std::span<std::byte> data, size_t offset /* = 0 */, std::string_view label /* = "Stylizer Buffer" */) {
		static webgpu::buffer buffer;
		return buffer = create_and_write_buffer(usage, data, offset, label);
	}

	webgpu::render_pass device::create_render_pass(std::span<api::render_pass::color_attachment> colors, std::optional<api::render_pass::depth_stencil_attachment> depth /* = {} */, bool one_shot /* = false */, std::string_view label /* = "Stylizer Render Pass" */) {
		return webgpu::render_pass::create(*this, colors, depth, one_shot, label);
	}
	api::render_pass& device::create_render_pass(temporary_return_t, std::span<api::render_pass::color_attachment> colors, std::optional<api::render_pass::depth_stencil_attachment> depth /* = {} */, bool one_shot /* = false */, std::string_view label /* = "Stylizer Render Pass" */) {
		static webgpu::render_pass render_pass;
		return render_pass = create_render_pass(colors, depth, one_shot, label);
	}

	static_assert(device_concept<device>);
}