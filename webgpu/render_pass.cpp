#include "render_pass.hpp"
#include "render_pipeline.hpp"

namespace stylizer::api::webgpu {
	render_pass& render_pass::bind_render_pipeline(api::device& device, const api::render_pipeline& pipeline_) {
		render_used = true;
		auto& pipeline = confirm_wgpu_type<webgpu::render_pipeline>(pipeline_);
		pass.setPipeline(pipeline.pipeline);
		return *this;
	}
}