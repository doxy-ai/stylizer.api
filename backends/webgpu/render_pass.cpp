#include <webgpu/webgpu.h>

#include <cassert>

import std.compat;
import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;
import stylizer.graphics;

namespace stylizer::graphics::webgpu {

	render_pass render_pass::create(graphics::device& device_, std::span<const render_pass::color_attachment> colors, const std::optional<depth_stencil_attachment>& depth /* = {} */, bool one_shot /* = false */, const std::string_view label_ /* = "Stylizer Render Pass" */) {
		assert(colors.size() > 0);
		assert(!(depth.has_value() && depth->depth_clear_value.has_value()) || depth->depth_clear_value >= 0);
		assert(!(depth.has_value() && depth->depth_clear_value.has_value()) || depth->depth_clear_value <= 1);
		auto& device = confirm_webgpu_type<webgpu::device>(device_);

		render_pass out({colors.begin(), colors.end()}, depth);
		std::string label = out.label = label_;
		out.one_shot = one_shot;

		{
			label += "Encoder";
			WGPUCommandEncoderDescriptor d { .label = to_webgpu(label) };
			out.render_encoder = wgpuDeviceCreateCommandEncoder(device.device_, &d);
		}

		std::vector<WGPURenderPassColorAttachment> color_attachments; color_attachments.reserve(colors.size());
		for(auto& attach: colors) {
			assert(attach.texture || attach.view);
			auto& unconfirmed_view = attach.texture ? confirm_webgpu_type<webgpu::texture>(*attach.texture).full_view(device) : *attach.view;
			auto& view = confirm_webgpu_type<webgpu::texture_view>(unconfirmed_view);
			color_attachments.emplace_back(WGPURenderPassColorAttachment{
				.view = view.view,
				.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
				.resolveTarget = attach.resolve_target ? confirm_webgpu_type<webgpu::texture_view>(*attach.resolve_target).view : nullptr,
				.loadOp = attach.clear_value.has_value() ? WGPULoadOp_Clear : WGPULoadOp_Load,
				.storeOp = attach.should_store ? WGPUStoreOp_Store : WGPUStoreOp_Discard,
				.clearValue = attach.clear_value.has_value() ? to_webgpu(*attach.clear_value) : WGPUColor{},
			});
		}
		WGPURenderPassDepthStencilAttachment* depth_attachment = nullptr;
		if(depth) {
			assert(depth->texture || depth->view);
			auto& view = confirm_webgpu_type<webgpu::texture_view>(depth->texture ? confirm_webgpu_type<webgpu::texture>(*depth->texture).full_view(device) : *depth->view);
			bool hasStencil = depth->stencil.has_value();
			auto stencil = depth->stencil.value_or(depth_stencil_attachment::stencil_config{});
			auto loadOP = stencil.clear_value.has_value() ? WGPULoadOp_Clear : WGPULoadOp_Load;
			auto storeOP = stencil.should_store ? WGPUStoreOp_Store : WGPUStoreOp_Discard;

			static WGPURenderPassDepthStencilAttachment static_depth;
			static_depth = {
				.view = view.view,
				.depthLoadOp = depth->depth_clear_value.has_value() ? WGPULoadOp_Clear : WGPULoadOp_Load,
				.depthStoreOp = depth->should_store_depth ? WGPUStoreOp_Store : WGPUStoreOp_Discard,
				.depthClearValue = depth->depth_clear_value.has_value() ? *depth->depth_clear_value : 1,
				.depthReadOnly = depth->depth_readonly,
				.stencilLoadOp = hasStencil ? loadOP : WGPULoadOp_Undefined,
				.stencilStoreOp = hasStencil ? storeOP : WGPUStoreOp_Undefined,
				.stencilClearValue = static_cast<uint32_t>(stencil.clear_value.has_value() ? *stencil.clear_value : 0),
				.stencilReadOnly = stencil.readonly,
			};
			depth_attachment = &static_depth;
		}

		{
			WGPURenderPassDescriptor d = WGPU_RENDER_PASS_DESCRIPTOR_INIT;
			d.label = to_webgpu(label),
			d.colorAttachmentCount = color_attachments.size(),
			d.colorAttachments = color_attachments.data(),
			d.depthStencilAttachment = depth_attachment,
			d.occlusionQuerySet = nullptr,
			d.timestampWrites = nullptr,
			out.pass = wgpuCommandEncoderBeginRenderPass(out.render_encoder, &d);
		}

		return out;
	}

	graphics::render_pass& render_pass::bind_render_pipeline(graphics::device& device, const graphics::render_pipeline& pipeline_, bool release_on_submit /* =  false */)  {
		render_used = true;
		auto& pipeline = confirm_webgpu_type<webgpu::render_pipeline>(pipeline_);
		wgpuRenderPassEncoderSetPipeline(pass, pipeline.pipeline);
		if(release_on_submit) deferred_to_release->connect([pipeline = std::move(pipeline)]() mutable {
			pipeline.release();
		});
		return *this;
	}

	graphics::render_pass& render_pass::bind_render_group(graphics::device& device, const graphics::bind_group& group_, std::optional<bool> release_on_submit /* = false */, std::optional<size_t> index_ /* = {} */)  {
		auto& group = confirm_webgpu_type<webgpu::bind_group>(group_);
		wgpuRenderPassEncoderSetBindGroup(pass, index_.value_or(group.index), group.group, 0, nullptr);
		if(release_on_submit.value_or(false)) deferred_to_release->connect([group = std::move(group)]() mutable {
			group.release();
		});
		return *this;
	}
	graphics::render_pass& render_pass::bind_vertex_buffer(graphics::device& device, size_t slot, const graphics::buffer& buffer_, std::optional<size_t> offset /* = 0 */, std::optional<size_t> size_ /* = {} */)  {
		render_used = true;
		auto& buffer = confirm_webgpu_type<webgpu::buffer>(buffer_);
		wgpuRenderPassEncoderSetVertexBuffer(pass, slot, buffer.buffer_, offset.value_or(0), size_.value_or(buffer.size()));
		return *this;
	}
	graphics::render_pass& render_pass::bind_index_buffer(graphics::device& device, const graphics::buffer& buffer_, std::optional<size_t> offset /* = 0 */, std::optional<size_t> size_ /* = {} */)  {
		render_used = true;
		auto& buffer = confirm_webgpu_type<webgpu::buffer>(buffer_);
		wgpuRenderPassEncoderSetIndexBuffer(pass, buffer.buffer_, WGPUIndexFormat_Uint32, offset.value_or(0), size_.value_or(buffer.size()));
		return *this;
	}
	graphics::render_pass& render_pass::draw(graphics::device& device, size_t vertex_count, std::optional<size_t> instance_count /* = 1 */, std::optional<size_t> first_vertex /* = 0 */, size_t first_instance /* = 0 */)  {
		render_used = true;
		wgpuRenderPassEncoderDraw(pass, vertex_count, instance_count.value_or(1), first_vertex.value_or(0), first_instance);
		return *this;
	}
	graphics::render_pass& render_pass::draw_indexed(graphics::device& device, size_t index_count, std::optional<size_t> instance_count /* = 1 */, std::optional<size_t> first_index /* = 0 */, std::optional<size_t> base_vertex /* = 0 */, size_t first_instance /* = 0 */)  {
		render_used = true;
		wgpuRenderPassEncoderDrawIndexed(pass, index_count, instance_count.value_or(1), first_index.value_or(0), base_vertex.value_or(0), first_instance);
		return *this;
	}

	webgpu::command_buffer render_pass::end(graphics::device& device) {
		if(compute_pass) wgpuComputePassEncoderEnd(compute_pass);
		command_buffer out;
		WGPUCommandBufferDescriptor d = WGPU_COMMAND_BUFFER_DESCRIPTOR_INIT;
		if(pre_encoder) out.pre = wgpuCommandEncoderFinish(pre_encoder, &d);
		if(compute_encoder) out.compute = wgpuCommandEncoderFinish(compute_encoder, &d);
		if(render_used) {
			wgpuRenderPassEncoderEnd(pass);
			out.render = wgpuCommandEncoderFinish(render_encoder, &d);
		}
		{ // TODO: Why does this have to be two steps?
			auto& deferred = *deferred_to_release;
			out.deferred_to_release = std::move(deferred);
		}
		return out;
	}
	graphics::command_buffer& render_pass::end(temporary_return_t, graphics::device& device)  {
		static command_buffer buffer;
		return buffer = end(device);
	}

	void render_pass::one_shot_submit(graphics::device& device)  {
		assert(one_shot);
		auto buffer = end(device);
		buffer.submit(device, true);
		this->release();
	}

	void render_pass::release()  {
		if(pre_encoder) wgpuCommandEncoderRelease(std::exchange(pre_encoder, nullptr));
		if(compute_encoder) wgpuCommandEncoderRelease(std::exchange(compute_encoder, nullptr));
		if(compute_pass) wgpuComputePassEncoderRelease(std::exchange(compute_pass, nullptr));
		// TODO: ^^^ Calling release on command encoder infinitely recurses?
		if(render_encoder) wgpuCommandEncoderRelease(std::exchange(render_encoder, nullptr));
		if(pass) wgpuRenderPassEncoderRelease(std::exchange(pass, nullptr));
		(*deferred_to_release)();
	}

}