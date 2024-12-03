#include "device.hpp"
#include "surface.hpp"

namespace stylizer::api::webgpu {
	device device::create_default(const create_config& config) {
		device out;
		out.adapter = get_instance().requestAdapter(WGPURequestAdapterOptions{
			.compatibleSurface = config.compatible_surface ? confirm_wgpu_type<webgpu::surface>(*config.compatible_surface).surface_ : nullptr,
			.powerPreference = config.high_performance ? wgpu::PowerPreference::HighPerformance : wgpu::PowerPreference::LowPower,
		});
		out.device_ = out.adapter.requestDevice({});
		out.queue = out.device_.getQueue();
		return out;
	}
}