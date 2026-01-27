module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:render_pipeline;

import :device;
import :bind_group;
import :shader;

namespace stylizer::graphics::webgpu {

	struct render_pipeline : public graphics::render_pipeline { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(render_pipeline); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pipeline);
		uint32_t type = magic_number;
		WGPURenderPipeline pipeline = nullptr;

		render_pipeline(render_pipeline&& o) { *this = std::move(o); }
		render_pipeline& operator=(render_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() const override { return pipeline; }

		static render_pipeline create(graphics::device& device_, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline");

		// Implemented in render_pass.cppm
		static render_pipeline create_from_compatible_render_pass(graphics::device& device, const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline");
		
		webgpu::bind_group create_bind_group(graphics::device& device_, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group");
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group") override;

		void release() override;
		stylizer::auto_release<render_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(render_pipeline_concept<render_pipeline>);
}