module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:render_pass;

import :command_encoder;
import :render_pipeline;

namespace stylizer::graphics::webgpu {

	struct render_pass : public command_encoder_base<graphics::render_pass, render_pass> { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(render_pass); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pass);
		using super = webgpu::command_encoder_base<graphics::render_pass, render_pass>;

		// NOTE: Type gets inherited from command_encoder
		WGPUCommandEncoder render_encoder = nullptr;
		WGPURenderPassEncoder pass = nullptr;
		std::vector<color_attachment> color_attachments = {};
		std::optional<depth_stencil_attachment> depth_attachment = {};
		bool render_used = false;

		inline render_pass(std::vector<color_attachment> colors, std::optional<depth_stencil_attachment> depth): color_attachments(std::move(colors)), depth_attachment(depth) {}
		inline render_pass(render_pass&& o) { *this = std::move(o); }
		inline render_pass& operator=(render_pass&& o) {
			super::operator=(std::move(o));
			render_encoder = std::exchange(o.render_encoder, nullptr);
			pass = std::exchange(o.pass, nullptr);
			color_attachments = std::exchange(o.color_attachments, {});
			depth_attachment = std::exchange(o.depth_attachment, {});
			render_used = std::exchange(o.render_used, false);
			return *this;
		}
		inline operator bool() const override { return super::operator bool() || render_encoder || pass; }

		static render_pass create(graphics::device& device_, std::span<const render_pass::color_attachment> colors, const std::optional<depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label_ = "Stylizer Render Pass");
		graphics::render_pass& bind_render_pipeline(graphics::device& device, const graphics::render_pipeline& pipeline_, bool release_on_submit =  false) override;

		graphics::render_pass& bind_render_group(graphics::device& device, const graphics::bind_group& group_, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override;
		graphics::render_pass& bind_vertex_buffer(graphics::device& device, size_t slot, const graphics::buffer& buffer_, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override;
		graphics::render_pass& bind_index_buffer(graphics::device& device, const graphics::buffer& buffer_, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override;
		graphics::render_pass& draw(graphics::device& device, size_t vertex_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_vertex = 0, size_t first_instance = 0) override;
		graphics::render_pass& draw_indexed(graphics::device& device, size_t index_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_index = 0, std::optional<size_t> base_vertex = 0, size_t first_instance = 0) override;

		webgpu::command_buffer end(graphics::device& device);
		graphics::command_buffer& end(temporary_return_t, graphics::device& device) override;

		void one_shot_submit(graphics::device& device) override;

		void release() override;
		stylizer::auto_release<render_pass> auto_release() { return std::move(*this); }
	};
	static_assert(render_pass_concept<render_pass>);
}