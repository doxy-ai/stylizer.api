#pragma once

#include "common.hpp"

namespace stylizer::api::webgpu {
	struct texture;
	struct buffer;
	struct shader;
	struct command_encoder;
	struct compute_pipeline;
	struct render_pass;
	struct render_pipeline;

	namespace compute {
		using pass = command_encoder;
		using pipeline = compute_pipeline;
	}
	namespace render {
		using pass = render_pass;
		using pipeline = render_pipeline;
	}

	struct device: public api::device { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(device);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::Adapter adapter = nullptr;
		wgpu::Device device_ = nullptr;
		wgpu::Queue queue = nullptr;

		inline device(device&& o) { *this = std::move(o); }
		inline device& operator=(device&& o) {
			adapter = std::exchange(o.adapter, nullptr);
			device_ = std::exchange(o.device_, nullptr);
			queue = std::exchange(o.queue, nullptr);
			return *this;
		}
		inline operator bool() const override { return adapter || device_; }

		static webgpu::device create_default(const webgpu::device::create_config& config = {});

		bool process_events() {
			// for (int i = 0 ; i < 5 ; ++i) {
				// std::cout << "Tick/Poll device..." << std::endl;
#if defined(WEBGPU_BACKEND_DAWN)
				return wgpuDeviceTick(device_);
#elif defined(WEBGPU_BACKEND_WGPU)
				return wgpuDevicePoll(device_, false, nullptr);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
				emscripten_sleep(1);
				return true;
#endif
			// }
		}
		bool wait(bool for_queues = true) override {; // Wait for device to finish
			if(!for_queues) return process_events();

			bool success, done = false;
			auto handle = queue.onSubmittedWorkDone([&done, &success](wgpu::QueueWorkDoneStatus status) {
				success = status == wgpu::QueueWorkDoneStatus::Success;
				// assert(success);
				done = true;
			});
			while(!done) process_events(); // TODO: Should we have a sleep in here?
			wgpuQueueOnSubmittedWorkDone(queue, nullptr, nullptr); // Clear the callback
			return success;
		}

		webgpu::texture create_texture(const api::texture::create_config& config = {});
		api::texture& create_texture(temporary_return_t, const api::texture::create_config& config = {}) override;
		webgpu::texture create_and_write_texture(std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config = {});
		api::texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config = {}) override;

		webgpu::buffer create_buffer(usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer");
		api::buffer& create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") override;
		webgpu::buffer create_and_write_buffer(usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer");
		api::buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") override;
		template<typename T> requires(!std::same_as<T, std::byte>) // NOTE: non temp return version can't compile since buffer isn't a defined type yet!
		api::buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write_buffer(usage, byte_span(data), offset, label);
		}

		webgpu::shader create_shader_from_spirv(slcross::spirv_view spirv, const std::string_view label = "Stylizer Shader");
		api::shader& create_shader_from_spirv(temporary_return_t, slcross::spirv_view spirv, const std::string_view label = "Stylizer Shader") override;

		webgpu::command_encoder create_command_encoder(bool one_shot = false, const std::string_view label = "Stylizer Command Encoder");
		api::command_encoder& create_command_encoder(temporary_return_t, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") override;

		webgpu::render_pass create_render_pass(std::span<const api::render_pass::color_attachment> colors, std::optional<api::render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass");
		api::render_pass& create_render_pass(temporary_return_t, std::span<const api::render_pass::color_attachment> colors, const std::optional<api::render_pass::depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") override;

		webgpu::compute_pipeline create_compute_pipeline(const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline");
		api::compute_pipeline& create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") override;
		
		webgpu::render_pipeline create_render_pipeline(const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, std::optional<depth_stencil_attachment> depth_attachment = {}, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline");
		api::render_pipeline& create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override;
		webgpu::render_pipeline create_render_pipeline_from_compatible_render_pass(const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline");
		api::render_pipeline& create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override;

		void release(bool static_sub_objects = false) override {
			if(static_sub_objects) {
				static auto_release<device> device = {};
				device.release(false);
				device = std::move(*this);
				return;
			}
			if(device_) std::exchange(device_, nullptr).release();
			if(adapter) std::exchange(adapter, nullptr).release();
		}
	};
	// static_assert(device_concept<device>); // NOTE: In .cpp file
}