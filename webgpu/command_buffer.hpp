#pragma once

#include "device.hpp"

namespace stylizer::api::webgpu {
	struct command_buffer: public api::command_buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_buffer);
		wgpu::CommandBuffer pre = nullptr;
		wgpu::CommandBuffer main = nullptr;

		command_buffer(command_buffer&& o) { *this = std::move(o); }
		command_buffer& operator=(command_buffer&& o) {
			pre = std::exchange(o.pre, nullptr);
			main = std::exchange(o.main, nullptr);
			return *this;
		}
		inline operator bool() { return pre || main; }

		void submit(api::device& device_, bool release = true) override {
			assert(*this);
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			device.queue.submit({pre, main});
		}

		void release() override {
			if(pre) std::exchange(pre, nullptr).release();
			if(main) std::exchange(main, nullptr).release();
		}
	};

	namespace detail {
		struct command_encoder: public api::command_encoder { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder);
			wgpu::CommandEncoder encoder = nullptr;

			command_encoder(command_encoder&& o) { *this = std::move(o); }
			command_encoder& operator=(command_encoder&& o) {
				encoder = std::exchange(o.encoder, nullptr);
				return *this;
			}
			inline operator bool() { return encoder; }
		};
	}
}