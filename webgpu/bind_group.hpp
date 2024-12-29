#pragma once

#include "device.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace stylizer::api::webgpu {
	struct bind_group: public api::bind_group { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(bind_group);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::BindGroup group;
		size_t index;

		bind_group(bind_group&& o) { *this = std::move(o); }
		bind_group& operator=(bind_group&& o) {
			group = std::exchange(o.group, nullptr);
			index = o.index;
			return *this;
		}
		inline operator bool() const override { return group; }

		static bind_group internal_create(webgpu::device& device, size_t index, wgpu::BindGroupLayout layout, std::span<const bind_group::binding> bindings) {
			uint32_t i = 0;
			std::vector<WGPUBindGroupEntry> entries; entries.reserve(bindings.size());
			for(auto& binding: bindings)
				switch (binding.index()) {
				break; case 0: {
					auto& bind = std::get<bind_group::buffer_binding>(binding);
					assert(bind.buffer);
					auto& buffer = confirm_wgpu_type<webgpu::buffer>(*bind.buffer);
					entries.emplace_back(WGPUBindGroupEntry{
						.binding = i++,
						.buffer = buffer.buffer_,
						.offset = bind.offset,
						.size = bind.size_override.value_or(buffer.size() - bind.offset),
					});
				}
				break; case 1: {
					auto& bind = std::get<bind_group::texture_binding>(binding);
					assert(bind.texture || bind.texture_view);
					auto& view = confirm_wgpu_type<webgpu::texture_view>(bind.texture 
						? confirm_wgpu_type<webgpu::texture>(*bind.texture).full_view(device) 
						: *bind.texture_view);
					assert(view.owning_texture);
					auto& texture = confirm_wgpu_type<webgpu::texture>(*view.owning_texture);
					entries.emplace_back(WGPUBindGroupEntry{
						.binding = i++,
						.textureView = view.view,
					});
					if(bind.sampled_override.value_or(texture.sampled())) {
						assert(texture.sampler);
						entries.emplace_back(WGPUBindGroupEntry{
							.binding = i++,
							.sampler = texture.sampler,
						});
					} 
				}
				}

			bind_group out;
			out.index = index;
			out.group = device.device_.createBindGroup(WGPUBindGroupDescriptor{
				.layout = layout,
				.entryCount = entries.size(),
				.entries = entries.data(),
			});
			return out;
		}

		void release() override {
			if(group) std::exchange(group, nullptr).release();
		}
	};
}