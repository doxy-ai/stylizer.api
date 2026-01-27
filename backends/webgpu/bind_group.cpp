#include <webgpu/webgpu.h>
#include <cassert>

import std.compat;
import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;

namespace stylizer::graphics::webgpu {

	bind_group bind_group::internal_create(webgpu::device& device, size_t index, WGPUBindGroupLayout layout, std::span<const binding> bindings, std::string_view label) {
		uint32_t i = 0;
		std::vector<WGPUBindGroupEntry> entries;
		entries.reserve(bindings.size());
		for (auto& binding : bindings)
			switch (binding.index()) {
				break;
			case 0: {
				auto& bind = std::get<bind_group::buffer_binding>(binding);
				assert(bind.buffer);
				auto& buffer = confirm_webgpu_type<webgpu::buffer>(*bind.buffer);
				entries.emplace_back(WGPUBindGroupEntry {
					.binding = i++,
					.buffer = buffer.buffer_,
					.offset = bind.offset,
					.size = bind.size_override.value_or(buffer.size() - bind.offset),
				});
			} break;
			case 1: {
				auto& bind = std::get<bind_group::texture_binding>(binding);
				assert(bind.texture || bind.texture_view);
				auto& view = confirm_webgpu_type<webgpu::texture_view>(bind.texture
					? confirm_webgpu_type<webgpu::texture>(*bind.texture).full_view(device)
					: *bind.texture_view);
				assert(view.owning_texture);
				auto& texture = confirm_webgpu_type<webgpu::texture>(*view.owning_texture);
				entries.emplace_back(WGPUBindGroupEntry {
					.binding = i++,
					.textureView = view.view,
				});
				if (bind.sampled_override.value_or(texture.sampled())) {
					assert(texture.sampler);
					entries.emplace_back(WGPUBindGroupEntry {
						.binding = i++,
						.sampler = texture.sampler,
					});
				}
			}
			}

		bind_group out;
		out.index = index;
		WGPUBindGroupDescriptor d = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
		d.label = to_webgpu(label);
		d.layout = layout,
		d.entryCount = entries.size(),
		d.entries = entries.data(),
		out.group = wgpuDeviceCreateBindGroup(device.device_, &d);
		return out;
	}

	void bind_group::release() {
		if(group) wgpuBindGroupRelease(std::exchange(group, nullptr));
	}
}