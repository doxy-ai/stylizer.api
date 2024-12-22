#include "render_pass.hpp"
#include "graphics_pipeline.hpp"

namespace stylizer::api::webgpu {
	render_pass& render_pass::bind_pipeline(const api::graphics_pipeline& pipeline_) {
		auto& pipeline = confirm_wgpu_type<webgpu::graphics_pipeline>(pipeline_);
		pass.setPipeline(pipeline.pipeline);
		return *this;
	}
}