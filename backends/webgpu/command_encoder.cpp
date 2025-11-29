#include "common.hpp"

namespace stylizer::api::webgpu {
	template<typename Tapi_return, typename Twebgpu_return>
	Twebgpu_return command_encoder_base<Tapi_return, Twebgpu_return>::create(api::device& device_, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);

		Twebgpu_return out;
		out.label = label;
		out.one_shot = one_shot;
		return out;
	}

	template<typename Tapi_return, typename Twebgpu_return>
	WGPUCommandEncoder command_encoder_base<Tapi_return, Twebgpu_return>::maybe_create_pre_encoder(webgpu::device& device) {
		std::string label = this->label + " Pre Encoder";
		WGPUCommandEncoderDescriptor d = {.label = label.c_str()};
		if(!pre_encoder) pre_encoder = wgpuDeviceCreateCommandEncoder(device.device_, &d);
		return pre_encoder;
	}

	template<typename Tapi_return, typename Twebgpu_return>
	WGPUComputePassEncoder command_encoder_base<Tapi_return, Twebgpu_return>::maybe_create_compute_pass(webgpu::device& device) {
		if(!compute_encoder) {
			std::string label = this->label + " Compute Encoder";
			WGPUCommandEncoderDescriptor d{.label = label.c_str()};
			compute_encoder = wgpuDeviceCreateCommandEncoder(device.device_, &d);
		}
		if(!compute_pass) {
			std::string label = this->label + " Compute Pass";
			WGPUComputePassDescriptor d{.label = label.c_str()};
			compute_pass = wgpuCommandEncoderBeginComputePass(compute_encoder, &d);
		}
		return compute_pass;
	}

	// Defined in buffer.cpp
	void copy_buffer_to_buffer_impl(WGPUCommandEncoder e, webgpu::buffer& destination, const webgpu::buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {});

	template<typename Tapi_return, typename Twebgpu_return>
	Tapi_return& command_encoder_base<Tapi_return, Twebgpu_return>::copy_buffer_to_buffer(api::device& device_, api::buffer& destination, const api::buffer& source, std::optional<size_t> destination_offset /* = 0 */, std::optional<size_t> source_offset /* = 0 */, std::optional<size_t> size_override /* = {} */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		auto& dest = confirm_webgpu_type<webgpu::buffer>(destination);
		size_t dest_offset = destination_offset.value_or(0);
		size_t dest_size = size_override.value_or(dest.size() - dest_offset);
		auto& src = confirm_webgpu_type<webgpu::buffer>(source);
		size_t src_offset = source_offset.value_or(0);
		size_t src_size = size_override.value_or(src.size() - src_offset);
		assert(src_size <= dest_size);
		copy_buffer_to_buffer_impl(maybe_create_pre_encoder(device), dest, src, dest_offset, src_offset, src_size);
		return *(Tapi_return*)this;
	}

	// Defined in texture.cpp
	void copy_texture_to_texture_impl(WGPUCommandEncoder e, webgpu::texture& destination, const webgpu::texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {});

	template<typename Tapi_return, typename Twebgpu_return>
	Tapi_return& command_encoder_base<Tapi_return, Twebgpu_return>::copy_texture_to_texture(api::device& device_, api::texture& destination, const api::texture& source, std::optional<vec3u> destination_origin /* = { { 0, 0, 0 } } */, std::optional<vec3u> source_origin /* = { { 0, 0, 0 } } */, std::optional<vec3u> extent_override /* = {} */, std::optional<size_t> min_mip_level /* = 0 */, std::optional<size_t> mip_levels_override /* = {} */) {
		auto min_mip = min_mip_level.value_or(0);
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		auto& dest = confirm_webgpu_type<webgpu::texture>(destination);
		auto dest_origin = destination_origin.value_or({0, 0, 0});
		auto dest_extent = dest.size() - dest_origin;
		auto dest_mips = dest.mip_levels() - min_mip;
		// TODO: check origin < size
		auto& src = confirm_webgpu_type<webgpu::texture>(source);
		auto src_origin = destination_origin.value_or({0, 0, 0});
		auto src_extent = dest.size() - dest_origin;
		auto src_mips = src.mip_levels() - min_mip;
		if(!mip_levels_override.has_value()) assert(src_mips <= dest_mips);
		else assert(mip_levels_override.value() <= src_mips && mip_levels_override.value() <= dest_mips);
		copy_texture_to_texture_impl(maybe_create_pre_encoder(device), dest, src, destination_origin.value_or({ 0, 0, 0 }), source_origin.value_or({0, 0, 0}), src_extent, min_mip, mip_levels_override.value_or(src_mips));
		return *(Tapi_return*)this;
	}



	template<typename Tapi_return, typename Twebgpu_return>
	Tapi_return& command_encoder_base<Tapi_return, Twebgpu_return>::bind_compute_pipeline(api::device& device, const api::compute_pipeline& pipeline_, bool release_on_submit /* = false */) {
		auto& pipeline = confirm_webgpu_type<webgpu::compute_pipeline>(pipeline_);
		wgpuComputePassEncoderSetPipeline(maybe_create_compute_pass(confirm_webgpu_type<webgpu::device>(device)), pipeline.pipeline);
		if(release_on_submit) deferred_to_release->connect([pipeline = std::move(pipeline)]() mutable {
			pipeline.release();
		});
		return *(Tapi_return*)this;
	}

	template<typename Tapi_return, typename Twebgpu_return>
	Tapi_return& command_encoder_base<Tapi_return, Twebgpu_return>::bind_compute_group(api::device& device, const api::bind_group& group_, std::optional<bool> release_on_submit /* = false */, std::optional<size_t> index_override /* = {} */) {
		auto& group = confirm_webgpu_type<webgpu::bind_group>(group_);
		wgpuComputePassEncoderSetBindGroup(maybe_create_compute_pass(confirm_webgpu_type<webgpu::device>(device)), index_override.value_or(group.index), group.group, 0, nullptr);
		if(release_on_submit) deferred_to_release->connect([group = std::move(group)]() mutable {
			group.release();
		});
		return *(Tapi_return*)this;
	}

	template<typename Tapi_return, typename Twebgpu_return>
	Tapi_return& command_encoder_base<Tapi_return, Twebgpu_return>::dispatch_workgroups(api::device& device, vec3u workgroups) {
		wgpuComputePassEncoderDispatchWorkgroups(maybe_create_compute_pass(confirm_webgpu_type<webgpu::device>(device)), workgroups.x, workgroups.y, workgroups.z);
		return *(Tapi_return*)this;
	}

	template<typename Tapi_return, typename Twebgpu_return>
	command_buffer command_encoder_base<Tapi_return, Twebgpu_return>::end(api::device& device) {
		if(compute_pass) wgpuComputePassEncoderEnd(compute_pass);
		command_buffer out;
		WGPUCommandBufferDescriptor d = WGPU_COMMAND_BUFFER_DESCRIPTOR_INIT;
		if(pre_encoder) out.pre = wgpuCommandEncoderFinish(pre_encoder, &d);
		if(compute_encoder) out.compute = wgpuCommandEncoderFinish(compute_encoder, &d);
		{ // TODO: Why does this have to be two steps?
			auto& deferred = *deferred_to_release;
			out.deferred_to_release = std::move(deferred);
		}
		return out;
	}

	template<typename Tapi_return, typename Twebgpu_return>
	void command_encoder_base<Tapi_return, Twebgpu_return>::one_shot_submit(api::device& device) {
		assert(one_shot);
		auto buffer = end(device);
		buffer.submit(device, true);
		this->release();
	}

	template<typename Tapi_return, typename Twebgpu_return>
	void command_encoder_base<Tapi_return, Twebgpu_return>::release() {
		if(pre_encoder) wgpuCommandEncoderRelease(std::exchange(pre_encoder, nullptr));
		if(compute_encoder) wgpuCommandEncoderRelease(std::exchange(compute_encoder, nullptr));
		if(compute_pass) wgpuComputePassEncoderRelease(std::exchange(compute_pass, nullptr));
		(*deferred_to_release)();
	}

	static_assert(command_encoder_concept<command_encoder>);

	// Explicitly instantiate the two template instances we use!
	template class command_encoder_base<api::command_encoder, command_encoder>;
	template class command_encoder_base<api::render_pass, render_pass>;
} // namespace stylizer::api::webgpu