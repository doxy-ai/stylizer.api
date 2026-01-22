module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:render_pass;

import :device;
import :command_encoder;
import :command_buffer;

namespace stylizer::graphics::stub {

	export struct render_pass : public command_encoder_base<graphics::render_pass, render_pass> { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(render_pass); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pass);
		using super = stub::command_encoder_base<graphics::render_pass, render_pass>;

		// NOTE: Type gets inherited from command_encoder
		// inline render_pass(std::vector<color_attachment> colors, std::optional<depth_stencil_attachment> depth): color_attachments(std::move(colors)), depth_attachment(depth) {}
		inline render_pass(render_pass&& o) { *this = std::move(o); }
		inline render_pass& operator=(render_pass&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }


		static stub::render_pass create(graphics::device& device, std::span<const render_pass::color_attachment> colors, const std::optional<depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") { STYLIZER_THROW("Not implemented yet!"); }

		graphics::render_pass& bind_render_pipeline(graphics::device& device, const graphics::render_pipeline& pipeline, bool release_on_submit =  false) override { STYLIZER_THROW("Not implemented yet!"); }
		graphics::render_pass& bind_render_group(graphics::device& device, const graphics::bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		graphics::render_pass& bind_vertex_buffer(graphics::device& device, size_t slot, const graphics::buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		graphics::render_pass& bind_index_buffer(graphics::device& device, const graphics::buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::render_pass& draw(graphics::device& device, size_t vertex_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_vertex = 0, size_t first_instance = 0) override { STYLIZER_THROW("Not implemented yet!"); }
		graphics::render_pass& draw_indexed(graphics::device& device, size_t index_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_index = 0, std::optional<size_t> base_vertex = 0, size_t first_instance = 0) override { STYLIZER_THROW("Not implemented yet!"); }

		stub::command_buffer end(graphics::device& device) { STYLIZER_THROW("Not implemented yet!"); }
		graphics::command_buffer& end(temporary_return_t, graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		void one_shot_submit(graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<render_pass> auto_release() { return std::move(*this); }
	};
	static_assert(render_pass_concept<render_pass>);

	render_pass device::create_render_pass(std::span<const graphics::render_pass::color_attachment> colors, std::optional<graphics::render_pass::depth_stencil_attachment> depth /* = {} */, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Render Pass" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::render_pass& device::create_render_pass(graphics::temporary_return_t, std::span<const graphics::render_pass::color_attachment> colors, const std::optional<graphics::render_pass::depth_stencil_attachment>& depth /* = {} */, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Render Pass" */) { STYLIZER_THROW("Not implemented yet!"); }
}