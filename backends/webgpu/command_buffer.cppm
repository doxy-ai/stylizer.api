module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:command_buffer;

import :device;

namespace stylizer::graphics::webgpu {

	export struct command_buffer: public graphics::command_buffer { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_buffer); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_buffer);
		uint32_t type = magic_number;
		WGPUCommandBuffer pre = nullptr;
		WGPUCommandBuffer compute = nullptr;
		WGPUCommandBuffer render = nullptr;
		// TODO: Do we want to support sub command buffers?

		command_buffer(command_buffer&& o) { *this = std::move(o); }
		command_buffer& operator=(command_buffer&& o) {
			deferred_to_release = std::move(o.deferred_to_release);
			pre = std::exchange(o.pre, nullptr);
			compute = std::exchange(o.compute, nullptr);
			render = std::exchange(o.render, nullptr);
			return *this;
		}
		inline operator bool() const override { return pre || compute || render; }

		void submit(graphics::device& device_, bool release = true) override {
			if(!*this) { // If there is nothing to submit just skip!
				if (release) this->release();
				return;
			} 

			auto& device = confirm_webgpu_type<webgpu::device>(device_);
			std::vector<WGPUCommandBuffer> commands;
			commands.reserve(3);
			if (pre) commands.emplace_back(pre);
			if (compute) commands.emplace_back(compute);
			if (render) commands.emplace_back(render);
			wgpuQueueSubmit(device.queue, commands.size(), commands.data());
			if (release) this->release();
		}

		void release() override {
			if (pre) wgpuCommandBufferRelease(std::exchange(pre, nullptr));
			if (compute) wgpuCommandBufferRelease(std::exchange(compute, nullptr));
			if (render) wgpuCommandBufferRelease(std::exchange(render, nullptr));
			deferred_to_release();
		}
		stylizer::auto_release<command_buffer> auto_release() { return std::move(*this); }
	};
}