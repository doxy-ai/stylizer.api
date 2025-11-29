#include "common.hpp"

namespace stylizer::api::webgpu {

	bind_group internal_bind_group_create(webgpu::device& device, size_t index, WGPUBindGroupLayout layout, std::span<const bind_group::binding> bindings) {
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
		d.layout = layout,
		d.entryCount = entries.size(),
		d.entries = entries.data(),
		out.group = wgpuDeviceCreateBindGroup(device.device_, &d);
		return out;
	}

	compute_pipeline compute_pipeline::create(api::device& device_, pipeline::entry_point entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		assert(entry_point.shader);
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		auto& shader = confirm_webgpu_type<webgpu::shader>(*entry_point.shader);

		compute_pipeline out;
		WGPUComputePipelineDescriptor d = WGPU_COMPUTE_PIPELINE_DESCRIPTOR_INIT;
		d.label = to_webgpu(label);
		d.compute = {
			.module = shader.module,
			.entryPoint = to_webgpu(entry_point.entry_point_name)
		};
		out.pipeline = wgpuDeviceCreateComputePipeline(device.device_, &d);
		return out;
	}

	webgpu::bind_group compute_pipeline::create_bind_group(api::device& device_, size_t index, std::span<const bind_group::binding> bindings) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		return internal_bind_group_create(device, index, wgpuComputePipelineGetBindGroupLayout(pipeline, index), bindings);
	}
	api::bind_group& compute_pipeline::create_bind_group(temporary_return_t, api::device& device, size_t index, std::span<const bind_group::binding> bindings) {
		static webgpu::bind_group group;
		return group = create_bind_group(device, index, bindings);
	}

	void compute_pipeline::release() {
		if (pipeline) wgpuComputePipelineRelease(std::exchange(pipeline, nullptr));
	}

	static_assert(compute_pipeline_concept<compute_pipeline>);
} // namespace stylizer::api::webgpu