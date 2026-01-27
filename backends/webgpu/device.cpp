#include "../../util/error_macros.hpp"
#include <webgpu/webgpu.h>

import std.compat;
import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;
import stylizer.errors;
import stylizer.auto_release;

namespace stylizer::graphics::webgpu {

	bool device::process_events() {
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
		emscripten_sleep(1);
#else
		wgpuDeviceTick(device_);
#endif
		return true;
	}

	bool device::tick(bool for_queues /* = true */) {
		if (!for_queues) return process_events();

		bool done = false;
		wgpuQueueOnSubmittedWorkDone(queue, {
			.mode = WGPUCallbackMode_AllowProcessEvents,
			.callback = [](WGPUQueueWorkDoneStatus status, WGPUStringView message, WGPU_NULLABLE void* userdata, WGPU_NULLABLE void*){
				bool& done = *(bool*)userdata;
				switch(status){
				case WGPUQueueWorkDoneStatus_CallbackCancelled: [[fallthrough]];
				case WGPUQueueWorkDoneStatus_Error:
					STYLIZER_THROW("Queue submit failed!");
				break; case WGPUQueueWorkDoneStatus_Success:
				case WGPUQueueWorkDoneStatus_Force32:
					done = true;
				
				}
			}, .userdata1 = &done, .userdata2 = nullptr
		});
		while(!done) process_events();
		wgpuQueueOnSubmittedWorkDone(queue, {.callback = nullptr, .userdata1 = nullptr, .userdata2 = nullptr}); // Clear the callback
		return true;
	}

	void device::release(bool static_sub_objects /* = false */) {
		if (static_sub_objects) {
			static stylizer::auto_release<device> device = {};
			device.release(false);
			device = std::move(*this);
			return;
		}
		if (device_) wgpuDeviceRelease(std::exchange(device_, nullptr));
		if (adapter) wgpuAdapterRelease(std::exchange(adapter, nullptr));
	}
} // namespace stylizer::graphics::webgpu