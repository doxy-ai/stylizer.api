#pragma once

#include "common.hpp"

namespace stylizer::api::webgpu {
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

		static webgpu::device create_default(webgpu::device::create_config config);

		void release(bool static_sub_objects = false) override {
			if(static_sub_objects) {
				static auto_release<device> device = {};
				device.release(false);
				device = std::move(*this);
				return;
			}
			if(device_) device_.release();
			if(adapter) adapter.release();
		}
	};
	static_assert(device_concept<device>);
}