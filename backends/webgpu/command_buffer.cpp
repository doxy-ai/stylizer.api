#include <webgpu/webgpu.h>

import std.compat;
import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;
import stylizer.graphics;

namespace stylizer::graphics::webgpu {

	void command_buffer::submit(graphics::device& device_, bool release /* = true */) {
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

	void command_buffer::release() {
		if (pre) wgpuCommandBufferRelease(std::exchange(pre, nullptr));
		if (compute) wgpuCommandBufferRelease(std::exchange(compute, nullptr));
		if (render) wgpuCommandBufferRelease(std::exchange(render, nullptr));
		deferred_to_release();
	}
	
}