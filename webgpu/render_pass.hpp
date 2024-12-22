#pragma once

#include "command_encoder.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace stylizer::api::webgpu {
	struct render_pass: public webgpu::command_encoder, public api::render_pass { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pass);
		using super = webgpu::command_encoder;

		wgpu::CommandEncoder render_encoder = nullptr;
		wgpu::RenderPassEncoder pass = nullptr;
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


		static render_pass create(api::device& device_, std::span<const render_pass::color_attachment> colors, const std::optional<depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") {
			assert(colors.size() > 0);
			assert(!(depth.has_value() && depth->depth_clear_value.has_value()) || depth->depth_clear_value >= 0);
			assert(!(depth.has_value() && depth->depth_clear_value.has_value()) || depth->depth_clear_value <= 1);
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			render_pass out({colors.begin(), colors.end()}, depth);
			out.label = label;
			out.one_shot = one_shot;
			out.render_encoder = device.device_.createCommandEncoder(WGPUCommandEncoderDescriptor{.label = (out.label + " Encoder").c_str()});

			std::vector<WGPURenderPassColorAttachment> color_attachments; color_attachments.reserve(colors.size());
			for(auto& attach: colors) {
				assert(attach.texture || attach.view);
				auto& view = confirm_wgpu_type<webgpu::texture_view>(attach.texture ? confirm_wgpu_type<webgpu::texture>(*attach.texture).full_view(device) : *attach.view);
				color_attachments.emplace_back(WGPURenderPassColorAttachment{
					.view = view.view,
					.resolveTarget = attach.resolve_target ? confirm_wgpu_type<webgpu::texture_view>(*attach.resolve_target).view : nullptr,
					.loadOp = attach.clear_value.has_value() ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
					.storeOp = attach.should_store ? wgpu::StoreOp::Store : wgpu::StoreOp::Discard,
					.clearValue = attach.clear_value.has_value() ? to_wgpu(*attach.clear_value) : to_wgpu(color32{}),
				});
			}
			WGPURenderPassDepthStencilAttachment* depth_attachment = nullptr;
			if(depth) {
				assert(depth->texture || depth->view);
				auto& view = confirm_wgpu_type<webgpu::texture_view>(depth->texture ? confirm_wgpu_type<webgpu::texture>(*depth->texture).full_view(device) : *depth->view);
				static WGPURenderPassDepthStencilAttachment static_depth;
				static_depth = {
					.view = view.view,
					.depthLoadOp = depth->depth_clear_value.has_value() ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
					.depthStoreOp = depth->should_store_depth ? wgpu::StoreOp::Store : wgpu::StoreOp::Discard,
					.depthClearValue = depth->depth_clear_value.has_value() ? *depth->depth_clear_value : 1,
					.depthReadOnly = depth->depth_readonly,
					.stencilLoadOp = depth->stencil_clear_value.has_value() ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
					.stencilStoreOp = depth->should_store_stencil ? wgpu::StoreOp::Store : wgpu::StoreOp::Discard,
					.stencilClearValue = static_cast<uint32_t>(depth->stencil_clear_value.has_value() ? *depth->stencil_clear_value : 0),
					.stencilReadOnly = depth->stencil_readonly,
				};
				depth_attachment = &static_depth;
			}
			out.pass = out.render_encoder.beginRenderPass(WGPURenderPassDescriptor{
				.label = cstring_from_view(label),
				.colorAttachmentCount = color_attachments.size(),
				.colorAttachments = color_attachments.data(),
				.depthStencilAttachment = depth_attachment,
				.occlusionQuerySet = nullptr,
				.timestampWrites = nullptr,
			});

			return out;
		}

		api::command_encoder& copy_buffer_to_buffer(api::device& device, api::buffer& destination, const api::buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) override {
			return super::copy_buffer_to_buffer(device, destination, source, destination_offset, source_offset, size_override);
		}
		api::command_encoder& copy_texture_to_texture(api::device& device, api::texture& destination, const api::texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override {
			return super::copy_texture_to_texture(device, destination, source, destination_origin, source_origin, extent_override, min_mip_level, mip_levels_override);
		}

		api::command_encoder& bind_compute_pipeline(api::device& device, const api::compute_pipeline& pipeline) override {
			return super::bind_compute_pipeline(device, pipeline);
		}
		api::command_encoder& bind_compute_group(api::device& device, const api::bind_group& group, std::optional<size_t> index_override = {}) override {
			return super::bind_compute_group(device, group, index_override);
		}
		api::command_encoder& dispatch_workgroups(api::device& device, vec3u workgroups) override {
			return super::dispatch_workgroups(device, workgroups);
		}

		render_pass& bind_render_pipeline(api::device& device, const api::render_pipeline& pipeline) override;
		render_pass& bind_render_group(api::device& device, const api::bind_group& group_, std::optional<size_t> index_override = {}) override {
			render_used = true;
			auto& group = confirm_wgpu_type<webgpu::bind_group>(group_);
			pass.setBindGroup(index_override.value_or(group.index), group.group, 0, nullptr);
			return *this;
		}
		render_pass& bind_vertex_buffer(api::device& device, size_t slot, const api::buffer& buffer_, size_t offset = 0, std::optional<size_t> size_override = {}) override {
			render_used = true;
			auto& buffer = confirm_wgpu_type<webgpu::buffer>(buffer_);
			pass.setVertexBuffer(slot, buffer.buffer_, offset, size_override.value_or(buffer.size()));
			return *this;
		}
		render_pass& bind_index_buffer(api::device& device, const api::buffer& buffer_, size_t offset = 0, std::optional<size_t> size_override = {}) override {
			render_used = true;
			auto& buffer = confirm_wgpu_type<webgpu::buffer>(buffer_);
			pass.setIndexBuffer(buffer.buffer_, wgpu::IndexFormat::Uint32, offset, size_override.value_or(buffer.size()));
			return *this;
		}
		render_pass& draw(api::device& device, size_t vertex_count, size_t instance_count = 1, size_t first_vertex = 0, size_t first_instance = 0) override {
			render_used = true;
			pass.draw(vertex_count, instance_count, first_vertex, first_instance);
			return *this;
		}
		render_pass& draw_indexed(api::device& device, size_t index_count, size_t instance_count = 1, size_t first_index = 0, size_t base_vertex = 0, size_t first_instance = 0) override {
			render_used = true;
			pass.drawIndexed(index_count, instance_count, first_index, base_vertex, first_instance);
			return *this;
		}

		command_buffer end(api::device& device) {
			if(compute_pass) compute_pass.end();
			command_buffer out;
			if(pre_encoder) out.pre = pre_encoder.finish();
			if(compute_encoder) out.compute = compute_encoder.finish();
			if(render_used) {
				pass.end();
				out.render = render_encoder.finish();
			}
			return out;
		}
		command_buffer& end(temporary_return_t, api::device& device) override {
			static command_buffer buffer;
			return buffer = end(device);
		}

		void one_shot_submit(api::device& device) override {
			assert(one_shot);
			auto buffer = end(device);
			buffer.submit(device, true);
			this->release();
		}

		void release() override {
			if(pre_encoder) std::exchange(pre_encoder, nullptr).release();
			if(compute_encoder) std::exchange(compute_encoder, nullptr).release();
			if(compute_pass) std::exchange(compute_pass, nullptr).release();
			// TODO: ^^^ Calling release on command encoder infinitely recurses?
			if(render_encoder) std::exchange(render_encoder, nullptr).release();
			if(pass) std::exchange(pass, nullptr).release();
		}
	};
	static_assert(render_pass_concept<render_pass>);
}
