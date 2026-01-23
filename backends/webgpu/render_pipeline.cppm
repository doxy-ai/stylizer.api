module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:render_pipeline;

import :device;
import :bind_group;

namespace stylizer::graphics::webgpu {

    export struct render_pipeline : public graphics::render_pipeline { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(render_pipeline); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pipeline);
		uint32_t type = magic_number;

		render_pipeline(render_pipeline&& o) { *this = std::move(o); }
		render_pipeline& operator=(render_pipeline&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }


		static render_pipeline create(graphics::device& device, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline") { STYLIZER_THROW("Not implemented yet!"); }
		static render_pipeline create_from_compatible_render_pass(graphics::device& device, const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline") { STYLIZER_THROW("Not implemented yet!"); }

		webgpu::bind_group create_bind_group(graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group") { STYLIZER_THROW("Not implemented yet!"); }
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group") override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<render_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(render_pipeline_concept<render_pipeline>);

	render_pipeline device::create_render_pipeline(const graphics::pipeline::entry_points& entry_points, std::span<const graphics::color_attachment> color_attachments /* = {} */, std::optional<graphics::depth_stencil_attachment> depth_attachment /* = {} */, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::render_pipeline& device::create_render_pipeline(graphics::temporary_return_t, const graphics::pipeline::entry_points& entry_points, std::span<const graphics::color_attachment> color_attachments /* = {} */, const std::optional<graphics::depth_stencil_attachment>& depth_attachment /* = {} */, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) { STYLIZER_THROW("Not implemented yet!"); }
	render_pipeline device::create_render_pipeline_from_compatible_render_pass(const graphics::pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::render_pipeline& device::create_render_pipeline_from_compatible_render_pass(graphics::temporary_return_t, const graphics::pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) { STYLIZER_THROW("Not implemented yet!"); }
	
}