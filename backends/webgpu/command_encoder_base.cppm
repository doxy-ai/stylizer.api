module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:command_encoder_base;

// import :device;
import :command_buffer;
// import :buffer;
// import :texture;
import :compute_pipeline;

namespace stylizer::graphics::webgpu {

	export template<typename Tapi_return, typename Twebgpu_return>
	struct command_encoder_base : public Tapi_return { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder_base);
		uint32_t type = magic_number;
		std::shared_ptr<stylizer::signal<void()>> deferred_to_release = std::make_shared<stylizer::signal<void()>>();
		WGPUCommandEncoder pre_encoder = nullptr;
		WGPUCommandEncoder compute_encoder = nullptr;
		WGPUComputePassEncoder compute_pass = nullptr;
		std::string label;
		bool one_shot = false;

		inline command_encoder_base(command_encoder_base&& o) { *this = std::move(o); }
		inline command_encoder_base& operator=(command_encoder_base&& o) {
			deferred_to_release = std::move(o.deferred_to_release);
			pre_encoder = std::exchange(o.pre_encoder, nullptr);
			compute_encoder = std::exchange(o.compute_encoder, nullptr);
			compute_pass = std::exchange(o.compute_pass, nullptr);
			label = std::move(o.label);
			one_shot = o.one_shot;
			return *this;
		}
		inline operator bool() const override { return pre_encoder || compute_encoder || compute_pass; }
		Twebgpu_return&& move() { return std::move(*(Twebgpu_return*)this); }

		static Twebgpu_return create(graphics::device& device_, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") {
			auto& device = confirm_webgpu_type<webgpu::device>(device_);

			Twebgpu_return out;
			out.label = label;
			out.one_shot = one_shot;
			return out;
		}

		template<typename Tfunc>
		Twebgpu_return& defer(Tfunc&& func) { deferred_to_release->connect(std::move(func)); return *(Twebgpu_return*)this; }
		Tapi_return& defer(std::function<void()>&& func) override { deferred_to_release->connect(std::move(func)); return *(Tapi_return*)this; }

	protected:
		WGPUCommandEncoder maybe_create_pre_encoder(webgpu::device& device) {
			std::string label = this->label + " Pre Encoder";
			WGPUCommandEncoderDescriptor d = {.label = to_webgpu(label)};
			if(!pre_encoder) pre_encoder = wgpuDeviceCreateCommandEncoder(device.device_, &d);
			return pre_encoder;
		}

		WGPUComputePassEncoder maybe_create_compute_pass(webgpu::device& device) {
			if(!compute_encoder) {
				std::string label = this->label + " Compute Encoder";
				WGPUCommandEncoderDescriptor d{.label = to_webgpu(label)};
				compute_encoder = wgpuDeviceCreateCommandEncoder(device.device_, &d);
			}
			if(!compute_pass) {
				std::string label = this->label + " Compute Pass";
				WGPUComputePassDescriptor d{.label = to_webgpu(label)};
				compute_pass = wgpuCommandEncoderBeginComputePass(compute_encoder, &d);
			}
			return compute_pass;
		}
	public:

		Tapi_return& copy_buffer_to_buffer(graphics::device& device_, graphics::buffer& destination, const graphics::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override {
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

		Tapi_return& copy_buffer_to_texture(graphics::device& device, graphics::buffer& destination, const graphics::texture& source, std::optional<size_t> destination_offset = 0, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_buffer(graphics::device& device, graphics::texture& destination, const graphics::buffer& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<size_t> source_offset = 0, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		
		Tapi_return& copy_texture_to_texture(graphics::device& device_, graphics::texture& destination, const graphics::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override {
			auto min_mip = min_mip_level.value_or(0);
			auto& device = confirm_webgpu_type<webgpu::device>(device_);
			auto& dest = confirm_webgpu_type<webgpu::texture>(destination);
			auto dest_origin = destination_origin.value_or(vec3u{0, 0, 0});
			auto dest_extent = dest.size() - dest_origin;
			auto dest_mips = dest.mip_levels() - min_mip;
			// TODO: check origin < size
			auto& src = confirm_webgpu_type<webgpu::texture>(source);
			auto src_origin = destination_origin.value_or(vec3u{0, 0, 0});
			auto src_extent = dest.size() - dest_origin;
			auto src_mips = src.mip_levels() - min_mip;
			if(!mip_levels_override.has_value()) assert(src_mips <= dest_mips);
			else assert(mip_levels_override.value() <= src_mips && mip_levels_override.value() <= dest_mips);
			copy_texture_to_texture_impl(maybe_create_pre_encoder(device), dest, src, destination_origin.value_or(vec3u{ 0, 0, 0 }), source_origin.value_or(vec3u{0, 0, 0}), src_extent, min_mip, mip_levels_override.value_or(src_mips));
			return *(Tapi_return*)this;
		}

		Tapi_return& bind_compute_pipeline(graphics::device& device, const graphics::compute_pipeline& pipeline_, bool release_on_submit = false) override {
			auto& pipeline = confirm_webgpu_type<webgpu::compute_pipeline>(pipeline_);
			wgpuComputePassEncoderSetPipeline(maybe_create_compute_pass(confirm_webgpu_type<webgpu::device>(device)), pipeline.pipeline);
			if(release_on_submit) deferred_to_release->connect([pipeline = std::move(pipeline)]() mutable {
				pipeline.release();
			});
			return *(Tapi_return*)this;
		}

		Tapi_return& bind_compute_group(graphics::device& device, const graphics::bind_group& group_, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override {
			auto& group = confirm_webgpu_type<webgpu::bind_group>(group_);
			wgpuComputePassEncoderSetBindGroup(maybe_create_compute_pass(confirm_webgpu_type<webgpu::device>(device)), index_override.value_or(group.index), group.group, 0, nullptr);
			if(release_on_submit) deferred_to_release->connect([group = std::move(group)]() mutable {
				group.release();
			});
			return *(Tapi_return*)this;
		}

		Tapi_return& dispatch_workgroups(graphics::device& device, vec3u workgroups) override {
			wgpuComputePassEncoderDispatchWorkgroups(maybe_create_compute_pass(confirm_webgpu_type<webgpu::device>(device)), workgroups.x, workgroups.y, workgroups.z);
			return *(Tapi_return*)this;
		}

		command_buffer end(graphics::device& device) {
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

		graphics::command_buffer& end(temporary_return_t, graphics::device& device) override {
			static command_buffer buffer;
			return buffer = end(device);
		}

		void one_shot_submit(graphics::device& device) override {
			assert(one_shot);
			auto buffer = end(device);
			buffer.submit(device, true);
			this->release();
		}

		void release() override {
			if(pre_encoder) wgpuCommandEncoderRelease(std::exchange(pre_encoder, nullptr));
			if(compute_encoder) wgpuCommandEncoderRelease(std::exchange(compute_encoder, nullptr));
			if(compute_pass) wgpuComputePassEncoderRelease(std::exchange(compute_pass, nullptr));
			(*deferred_to_release)();
		}
	};
}