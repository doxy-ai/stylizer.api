#include <webgpu/webgpu.h>

#include <cassert>

import std.compat;
import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;
import stylizer.graphics;

namespace stylizer::graphics::webgpu {

	compute_pipeline compute_pipeline::create(graphics::device& device_, pipeline::entry_point entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
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

	webgpu::bind_group compute_pipeline::create_bind_group(graphics::device& device_, size_t index, std::span<const bind_group::binding> bindings, std::string_view label /* = "Stylizer Bind Group" */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		return bind_group::internal_create(device, index, wgpuComputePipelineGetBindGroupLayout(pipeline, index), bindings, label);
	}
	graphics::bind_group& compute_pipeline::create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label /* = "Stylizer Bind Group" */) {
		static webgpu::bind_group group;
		return group = create_bind_group(device, index, bindings, label);
	}

	void compute_pipeline::release() {
		if (pipeline) wgpuComputePipelineRelease(std::exchange(pipeline, nullptr));
	}

}