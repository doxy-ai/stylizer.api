#pragma once

#include "common.hpp"

namespace stylizer::api::webgpu {
	struct texture;
	struct render_pass;

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
		inline operator bool() { return adapter || device_; }

		static webgpu::device create_default(const webgpu::device::create_config& config = {});

		void process_events() {
			// for (int i = 0 ; i < 5 ; ++i) {
				// std::cout << "Tick/Poll device..." << std::endl;
#if defined(WEBGPU_BACKEND_DAWN)
				wgpuDeviceTick(device_);
#elif defined(WEBGPU_BACKEND_WGPU)
				wgpuDevicePoll(device_, false, nullptr);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
				emscripten_sleep(1);
#endif
			// }
		}

		webgpu::texture create_texture(const api::texture::create_config& config = {});
		api::texture& create_texture(temporary_return_t, const api::texture::create_config& config = {}) override;
		webgpu::texture create_and_write_texture(std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config = {});
		api::texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config = {}) override;

		webgpu::render_pass create_render_pass(std::span<api::render_pass::color_attachment> colors, std::optional<api::render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, std::string_view label = "Stylizer Render Pass");
		api::render_pass& create_render_pass(temporary_return_t, std::span<api::render_pass::color_attachment> colors, std::optional<api::render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, std::string_view label = "Stylizer Render Pass") override;

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