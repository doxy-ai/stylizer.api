module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:compute_pipeline;

import :device;
import :bind_group;
import :shader;

namespace stylizer::graphics::webgpu {

	export struct pipeline : public graphics::pipeline { };

	export struct compute_pipeline: public graphics::compute_pipeline { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(compute_pipeline); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(compute_pipeline);
		uint32_t type = magic_number;
		WGPUComputePipeline pipeline = nullptr;

		compute_pipeline(compute_pipeline&& o) { *this = std::move(o); }
		compute_pipeline& operator=(compute_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() const override { return pipeline; }

		static compute_pipeline create(graphics::device& device_, pipeline::entry_point entry_point, const std::string_view label = "Stylizer Compute Pipeline") {
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

		webgpu::bind_group create_bind_group(graphics::device& device_, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") {
			auto& device = confirm_webgpu_type<webgpu::device>(device_);
			return bind_group::internal_create(device, index, wgpuComputePipelineGetBindGroupLayout(pipeline, index), bindings, label);
		}
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") override {
			static webgpu::bind_group group;
			return group = create_bind_group(device, index, bindings, label);
		}

		void release() override {
			if (pipeline) wgpuComputePipelineRelease(std::exchange(pipeline, nullptr));
		}
		stylizer::auto_release<compute_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(compute_pipeline_concept<compute_pipeline>);

	webgpu::compute_pipeline device::create_compute_pipeline(const pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		return webgpu::compute_pipeline::create(*this, entry_point, label);
	}

	graphics::compute_pipeline& device::create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		static webgpu::compute_pipeline temp;
		return temp = create_compute_pipeline(entry_point, label);
	}
}