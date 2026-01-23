module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:bind_group;

import :device;
import :buffer;
import :texture;

namespace stylizer::graphics::webgpu {

	export struct bind_group: public graphics::bind_group { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(bind_group); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(bind_group);
		uint32_t type = magic_number;
		WGPUBindGroup group = nullptr;
		size_t index = 0;

		bind_group(bind_group&& o) { *this = std::move(o); }
		bind_group& operator=(bind_group&& o) {
			group = std::exchange(o.group, nullptr);
			index = o.index;
			return *this;
		}
		inline operator bool() const override { return group; }

		static bind_group internal_create(webgpu::device& device, size_t index, WGPUBindGroupLayout layout, std::span<const binding> bindings, std::string_view label) {
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

		void release() override {
			if(group) wgpuBindGroupRelease(std::exchange(group, nullptr));
		}
		stylizer::auto_release<bind_group> auto_release() { return std::move(*this); }
	};
}