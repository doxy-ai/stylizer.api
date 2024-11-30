#include "device.hpp"
#include "surface.hpp"

namespace stylizer::api::webgpu {
	device device::create_default(create_config config) {
		device out;
		out.adapter = get_instance().requestAdapter(WGPURequestAdapterOptions{
			.compatibleSurface = confirm_wgpu_type<webgpu::surface>(*config.surface).surface_,
			.powerPreference = config.high_performance ? wgpu::PowerPreference::HighPerformance : wgpu::PowerPreference::LowPower,
		});
		out.device_ = out.adapter.requestDevice({});
		out.queue = out.device_.getQueue();
		return out;
	}
}