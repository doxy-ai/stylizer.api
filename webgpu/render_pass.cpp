#include "render_pass.hpp"
#include "render_pipeline.hpp"

namespace stylizer::api::webgpu {
	render_pass& render_pass::bind_render_pipeline(api::device& device, const api::render_pipeline& pipeline_, bool release_on_submit /*=  false */) {
		render_used = true;
		auto& pipeline = confirm_wgpu_type<webgpu::render_pipeline>(pipeline_);
		pass.setPipeline(pipeline.pipeline);
		if(release_on_submit) deferred_to_release.emplace_back([pipeline = std::move(pipeline)]() mutable {
			pipeline.release();
		});
		return *this;
	}
}