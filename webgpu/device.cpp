#include "device.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "render_pass.hpp"

namespace stylizer::api::webgpu {
	device device::create_default(const create_config& config) {
		device out;
		out.adapter = get_instance().requestAdapter(WGPURequestAdapterOptions{
			.compatibleSurface = config.compatible_surface ? confirm_wgpu_type<webgpu::surface>(*config.compatible_surface).surface_ : nullptr,
			.powerPreference = config.high_performance ? wgpu::PowerPreference::HighPerformance : wgpu::PowerPreference::LowPower,
		});
		WGPUFeatureName float32filterable = WGPUFeatureName_Float32Filterable;
		out.device_ = out.adapter.requestDevice(WGPUDeviceDescriptor{
			.label = "Stylizer Device",
			.requiredFeatureCount = 1,
			.requiredFeatures = &float32filterable,
			.requiredLimits = nullptr,
			.defaultQueue = { .label = "Stylizer Queue" },
		});
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

	webgpu::texture device::create_and_write_texture(std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config /* = {} */) {
		return webgpu::texture::create_and_write(*this, data, layout, config);
	}
	api::texture& device::create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config /* = {} */) {
		static webgpu::texture texture;
		return texture = create_and_write_texture(data, layout, config);
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