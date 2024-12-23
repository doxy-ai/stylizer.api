#pragma once

#include "bind_group.hpp"
#include "command_buffer.hpp"
#include "compute_pipeline.hpp"
#include "texture.hpp"

namespace stylizer::api::webgpu {
	struct command_encoder: public api::command_encoder { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		event<void> deferred_to_release;
		wgpu::CommandEncoder pre_encoder = nullptr;
		wgpu::CommandEncoder compute_encoder = nullptr;
		wgpu::ComputePassEncoder compute_pass = nullptr;
		std::string label;
		bool one_shot = false;

		inline command_encoder(command_encoder&& o) { *this = std::move(o); }
		inline command_encoder& operator=(command_encoder&& o) {
			deferred_to_release = std::move(o.deferred_to_release);
			pre_encoder = std::exchange(o.pre_encoder, nullptr);
			compute_encoder = std::exchange(o.compute_encoder, nullptr);
			compute_pass = std::exchange(o.compute_pass, nullptr);
			label = std::move(o.label);
			one_shot = o.one_shot;
			return *this;
		}
		inline operator bool() const override { return pre_encoder || compute_encoder || compute_pass; }
		command_encoder&& move() { return std::move(*this); }


		static command_encoder create(api::device& device_, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			command_encoder out;
			out.label = label;
			out.one_shot = one_shot;
			return out;
		}

		template<typename Tfunc>
		webgpu::command_encoder& defer(Tfunc&& func) { deferred_to_release.emplace_back(std::move(func)); return *this; }
		api::command_encoder& defer(std::function<void()>&& func) override { deferred_to_release.emplace_back(std::move(func)); return *this; }

	protected:
		wgpu::CommandEncoder maybe_create_pre_encoder(webgpu::device& device) {
			if(!pre_encoder) pre_encoder = device.device_.createCommandEncoder(WGPUCommandEncoderDescriptor{.label = (label + " Pre Encoder").c_str()});
			return pre_encoder;
		}

		wgpu::ComputePassEncoder maybe_create_compute_pass(webgpu::device& device) {
			if(!compute_encoder) compute_encoder = device.device_.createCommandEncoder(WGPUCommandEncoderDescriptor{.label = (label + " Compute Encoder").c_str()});
			if(!compute_pass) compute_pass = compute_encoder.beginComputePass(WGPUComputePassDescriptor{
				.label = (label + " Compute Pass").c_str()
			});
			return compute_pass;
		}
	public:

		api::command_encoder& copy_buffer_to_buffer(api::device& device_, api::buffer& destination, const api::buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			auto& dest = confirm_wgpu_type<webgpu::buffer>(destination);
			auto& src = confirm_wgpu_type<webgpu::buffer>(source);
			webgpu::buffer::copy_buffer_to_buffer_impl(maybe_create_pre_encoder(device), dest, src, destination_offset, source_offset, size_override);
			return *this;
		}

		api::command_encoder& copy_texture_to_texture(api::device& device_, api::texture& destination, const api::texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			auto& dest = confirm_wgpu_type<webgpu::texture>(destination);
			auto& src = confirm_wgpu_type<webgpu::texture>(source);
			webgpu::texture::copy_texture_to_texture_impl(maybe_create_pre_encoder(device), dest, src, destination_origin, source_origin, extent_override, min_mip_level, mip_levels_override);
			return *this;
		}



		api::command_encoder& bind_compute_pipeline(api::device& device, const api::compute_pipeline& pipeline_, bool release_on_submit = false) override {
			auto& pipeline = confirm_wgpu_type<webgpu::compute_pipeline>(pipeline_);
			maybe_create_compute_pass(confirm_wgpu_type<webgpu::device>(device)).setPipeline(pipeline.pipeline);
			if(release_on_submit) deferred_to_release.emplace_back([pipeline = std::move(pipeline)]() mutable {
				pipeline.release();
			});
			return *this;
		}

		api::command_encoder& bind_compute_group(api::device& device, const api::bind_group& group_, bool release_on_submit = false, std::optional<size_t> index_override = {}) override {
			auto& group = confirm_wgpu_type<webgpu::bind_group>(group_);
			maybe_create_compute_pass(confirm_wgpu_type<webgpu::device>(device)).setBindGroup(index_override.value_or(group.index), group.group, 0, nullptr);
			if(release_on_submit) deferred_to_release.emplace_back([group = std::move(group)]() mutable {
				group.release();
			});
			return *this;
		}

		api::command_encoder& dispatch_workgroups(api::device& device, vec3u workgroups) override {
			maybe_create_compute_pass(confirm_wgpu_type<webgpu::device>(device)).dispatchWorkgroups(workgroups.x, workgroups.y, workgroups.z);
			return *this;
		}

		command_buffer end(api::device& device) {
			if(compute_pass) compute_pass.end();
			command_buffer out;
			if(pre_encoder) out.pre = pre_encoder.finish();
			if(compute_encoder) out.compute = compute_encoder.finish();
			out.deferred_to_release = std::move(deferred_to_release);
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
			deferred_to_release();
		}
	};
	static_assert(command_encoder_concept<command_encoder>);
}
