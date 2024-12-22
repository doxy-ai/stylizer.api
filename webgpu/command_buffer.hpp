#pragma once

#include "device.hpp"

namespace stylizer::api::webgpu {
	struct command_buffer: public api::command_buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_buffer);
		wgpu::CommandBuffer pre = nullptr;
		wgpu::CommandBuffer compute = nullptr;
		wgpu::CommandBuffer render = nullptr;

		command_buffer(command_buffer&& o) { *this = std::move(o); }
		command_buffer& operator=(command_buffer&& o) {
			deferred_to_release = std::move(o.deferred_to_release);
			pre = std::exchange(o.pre, nullptr);
			compute = std::exchange(o.compute, nullptr);
			render = std::exchange(o.render, nullptr);
			return *this;
		}
		inline operator bool() const override { return pre || compute || render; }

		void submit(api::device& device_, bool release = true) override {
			assert(*this); // Ensures there is at least one thing to submit!
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			std::vector<WGPUCommandBuffer> commands; commands.reserve(3);
			if(pre) commands.emplace_back(pre);
			if(compute) commands.emplace_back(compute);
			if(render) commands.emplace_back(render);
			device.queue.submit(commands);
			if(release) this->release();
		}

		void release() override {
			if(pre) std::exchange(pre, nullptr).release();
			if(compute) std::exchange(compute, nullptr).release();
			if(render) std::exchange(render, nullptr).release();
			deferred_to_release();
		}
	};
}