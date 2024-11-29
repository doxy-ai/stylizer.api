#include "common.hpp"

#include <utility>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu.hpp>

namespace stylizer::api::wgpu {
	struct device: public api::device { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(device);
		::wgpu::Instance instance = nullptr;
		::wgpu::Adapter adapter = nullptr;
		::wgpu::Device device_ = nullptr;
		::wgpu::Queue queue = nullptr;

		inline device(device&& o) { *this = std::move(o); }
		inline device& operator=(device&& o) { 
			instance = std::exchange(o.instance, nullptr);
			adapter = std::exchange(o.adapter, nullptr);
			device_ = std::exchange(o.device_, nullptr);
			queue = std::exchange(o.queue, nullptr);
			return *this;
		}
		inline operator bool() { return instance || adapter || device_; }

		static device create_default(create_config config) {
			device out;
			out.instance = ::wgpu::createInstance({});
			out.adapter = out.instance.requestAdapter(WGPURequestAdapterOptions{
				.compatibleSurface = nullptr,
				.powerPreference = config.high_performance ? ::wgpu::PowerPreference::HighPerformance : ::wgpu::PowerPreference::LowPower,
			});
			out.device_ = out.adapter.requestDevice({});
			out.queue = out.device_.getQueue();
			return out;
		}

		void release(bool static_sub_objects = false) {
			if(static_sub_objects) {
				static auto_release<device> device = {};
				device.release(false);
				device = std::move(*this);
				return;
			}
			if(device_) device_.release();
			if(adapter) adapter.release();
			if(instance) instance.release();
		}
	};
	static_assert(device_concept<device>);
}