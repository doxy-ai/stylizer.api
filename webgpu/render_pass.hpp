#pragma once

#include "command_buffer.hpp"
#include "common.hpp"
#include "texture.hpp"

namespace stylizer::api::webgpu {
	struct render_pass: public api::render_pass, public detail::command_encoder { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pass);
		wgpu::CommandEncoder pass_encoder = nullptr;
		wgpu::RenderPassEncoder pass = nullptr;
		std::vector<color_attachment> color_attachments = {};
		std::optional<depth_stencil_attachment> depth_attachment = {};

		inline render_pass(std::vector<color_attachment> colors, std::optional<depth_stencil_attachment> depth): color_attachments(std::move(colors)), depth_attachment(depth) {}
		inline render_pass(render_pass&& o) { *this = std::move(o); }
		inline render_pass& operator=(render_pass&& o) {
			encoder = std::exchange(o.encoder, nullptr);
			pass_encoder = std::exchange(o.pass_encoder, nullptr);
			pass = std::exchange(o.pass, nullptr);
			color_attachments = std::exchange(o.color_attachments, {});
			depth_attachment = std::exchange(o.depth_attachment, {});
			return *this;
		}
		inline operator bool() { return encoder || pass_encoder || pass; }

		static render_pass create(api::device& device_, std::span<render_pass::color_attachment> colors, std::optional<depth_stencil_attachment> depth = {}, bool one_shot = false, std::string_view label = "Stylizer Render Pass") {
			assert(colors.size() > 0);
			assert(!(depth.has_value() && depth->depth_clear_value.has_value()) || depth->depth_clear_value >= 0);
			assert(!(depth.has_value() && depth->depth_clear_value.has_value()) || depth->depth_clear_value <= 1);
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			render_pass out({colors.begin(), colors.end()}, depth);
			out.encoder = device.device_.createCommandEncoder(WGPUCommandEncoderDescriptor{.label = (std::string(label) + " Pre Encoder").c_str()});
			out.pass_encoder = device.device_.createCommandEncoder(WGPUCommandEncoderDescriptor{.label = (std::string(label) + " Encoder").c_str()});

			std::vector<WGPURenderPassColorAttachment> color_attachments; color_attachments.reserve(colors.size());
			for(auto& attach: colors) {
				assert(attach.texture);
				auto view = confirm_wgpu_type<webgpu::texture>(*attach.texture).view;
				color_attachments.emplace_back(WGPURenderPassColorAttachment{
					.view = view,
					.resolveTarget = attach.resolve_target ? confirm_wgpu_type<webgpu::texture>(*attach.resolve_target).view : nullptr,
					.loadOp = attach.clear_value.has_value() ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
					.storeOp = attach.should_store ? wgpu::StoreOp::Store : wgpu::StoreOp::Discard,
					.clearValue = attach.clear_value.has_value() ? to_wgpu(*attach.clear_value) : to_wgpu(color32{}),
				});
			}
			WGPURenderPassDepthStencilAttachment* depth_attachment = nullptr;
			if(depth) {
				assert(depth->texture);
				static WGPURenderPassDepthStencilAttachment static_depth;
				static_depth = {
					.view = confirm_wgpu_type<webgpu::texture>(*depth->texture).view,
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
			out.pass = out.encoder.beginRenderPass(WGPURenderPassDescriptor{
				.label = cstring_from_view(label),
				.colorAttachmentCount = color_attachments.size(),
				.colorAttachments = color_attachments.data(),
				.depthStencilAttachment = depth_attachment,
				.occlusionQuerySet = nullptr,
				.timestampWrites = nullptr,
			});

			return out;
		}

		render_pass& bind_pipeline(const api::graphics_pipeline& pipeline) override;
		render_pass& draw(size_t vertex_count, size_t instance_count = 1, size_t first_vertex = 0, size_t first_instance = 0) override {
			pass.draw(vertex_count, instance_count, first_vertex, first_instance);
			return *this;
		}

		command_buffer end(api::device& device) {
			pass.end();
			command_buffer out;
			out.pre = encoder.finish();
			out.main = pass_encoder.finish();
			return out;
		}
		command_buffer& end(temporary_return_t, api::device& device) override {
			static command_buffer buffer;
			return buffer = end(device);
		}

		void submit(api::device& device, bool release = true) override {
			auto buffer = end(device);
			buffer.submit(device, release);
			if(release) this->release();
		}

		void release() override {
			if(encoder) std::exchange(encoder, nullptr).release();
			if(pass_encoder) std::exchange(pass_encoder, nullptr).release();
			if(pass) std::exchange(pass, nullptr).release();
		}
	};
	static_assert(render_pass_concept<render_pass>);
}
