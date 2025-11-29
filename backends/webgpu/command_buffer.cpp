#include "common.hpp"

namespace stylizer::api::webgpu {

	void command_buffer::submit(api::device& device_, bool release /* = true */) {
		assert(*this); // Ensures there is at least one thing to submit!
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		std::vector<WGPUCommandBuffer> commands;
		commands.reserve(3);
		if (pre) commands.emplace_back(pre);
		if (compute) commands.emplace_back(compute);
		if (render) commands.emplace_back(render);
		wgpuQueueSubmit(device.queue, commands.size(), commands.data());
		if (release) this->release();
	}

	void command_buffer::release() {
		if (pre) wgpuCommandBufferRelease(std::exchange(pre, nullptr));
		if (compute) wgpuCommandBufferRelease(std::exchange(compute, nullptr));
		if (render) wgpuCommandBufferRelease(std::exchange(render, nullptr));
		deferred_to_release();
	}

	// static_assert(command_buffer_concept<command_buffer>);
} // namespace stylizer::api::webgpu