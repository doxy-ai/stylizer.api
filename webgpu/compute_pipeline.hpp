#pragma once

#include "shader.hpp"
#include "bind_group.hpp"

namespace stylizer::api::webgpu {
	struct compute_pipeline: public api::compute_pipeline { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(compute_pipeline);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::ComputePipeline pipeline = nullptr;

		compute_pipeline(compute_pipeline&& o) { *this = std::move(o); }
		compute_pipeline& operator=(compute_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() { return pipeline; }

		static compute_pipeline create(api::device& device_, pipeline::entry_point entry_point, std::string_view label = "Stylizer Compute Pipeline") {
			assert(entry_point.shader);
            auto& device = confirm_wgpu_type<webgpu::device>(device_);
            auto& shader = confirm_wgpu_type<webgpu::shader>(*entry_point.shader);

			compute_pipeline out;
            out.pipeline = device.device_.createComputePipeline(WGPUComputePipelineDescriptor{
                .label = cstring_from_view<0>(label),
                .compute = {
                    .module = shader.module,
                    .entryPoint = cstring_from_view<1>(entry_point.entry_point_name)
                }
            });
            return out;
		}

		webgpu::bind_group create_bind_group(api::device& device_, size_t index, std::span<const bind_group::binding> bindings) {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			return webgpu::bind_group::internal_create(device, index, pipeline.getBindGroupLayout(index), bindings);
		}
		api::bind_group& create_bind_group(temporary_return_t, api::device& device, size_t index, std::span<const bind_group::binding> bindings) override {
			static webgpu::bind_group group_;
			return group_ = create_bind_group(device, index, bindings);
		}

		void release() override {
			if(pipeline) std::exchange(pipeline, nullptr).release();
		}
	};
	static_assert(compute_pipeline_concept<compute_pipeline>);
}