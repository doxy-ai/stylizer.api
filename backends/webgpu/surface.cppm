module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:surface;

import :device;
import :texture;

namespace stylizer::graphics::webgpu {

	export struct surface : public graphics::surface { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(surface);  STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(surface);
		uint32_t type = magic_number;
		WGPUSurface surface_ = nullptr;

		inline surface(surface&& o) { *this = std::move(o); }
		inline surface& operator=(surface&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static webgpu::surface create_from_emscripten(const std::string_view canvas_id = "canvas") { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::surface create_from_cocoa(void* layer, void* window) { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::surface create_from_x11(void* display, void* window) { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::surface create_from_wayland(void* display, void* surface) { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::surface create_from_win32(void* window, void* instance) { STYLIZER_THROW("Not implemented yet!"); }

		config determine_optimal_default_config(graphics::device& device, vec2u size) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::surface& configure(graphics::device& device, const config& config) override { STYLIZER_THROW("Not implemented yet!"); }

		webgpu::texture next_texture(graphics::device& device) { STYLIZER_THROW("Not implemented yet!"); }
		graphics::texture& next_texture(temporary_return_t, graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		texture::format configured_texture_format(graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::surface& present(graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<surface> auto_release() { return std::move(*this); }
	};
	static_assert(surface_concept<surface>);

	webgpu::device device::create_default(const webgpu::device::create_config& config /* = {} */) {
		device out;

		WGPURequestAdapterOptions options = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
		options.featureLevel = WGPUFeatureLevel_Core;
		options.powerPreference = config.high_performance ? WGPUPowerPreference_HighPerformance : WGPUPowerPreference_LowPower;
		options.compatibleSurface = config.compatible_surface ? confirm_webgpu_type<webgpu::surface>(*config.compatible_surface).surface_ : nullptr;
		wait_for_future(wgpuInstanceRequestAdapter(get_common_instance(), &options, {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata, void*){
				WGPUAdapter& out = *(WGPUAdapter*)userdata;
				switch (status) {
				case WGPURequestAdapterStatus_CallbackCancelled: [[fallthrough]];
				case WGPURequestAdapterStatus_Unavailable: [[fallthrough]];
				case WGPURequestAdapterStatus_Error:
					STYLIZER_THROW(from_webgpu(message));
					[[fallthrough]];
				case WGPURequestAdapterStatus_Success: [[fallthrough]];
				case WGPURequestAdapterStatus_Force32:
					out = adapter;
				break;
				}
			},
			.userdata1 = &out.adapter, .userdata2 = nullptr
		}));

		WGPUFeatureName float32filterable = WGPUFeatureName_Float32Filterable;
		WGPUDeviceDescriptor device = WGPU_DEVICE_DESCRIPTOR_INIT;
		device.label = to_webgpu(config.label);
		device.requiredFeatureCount = 1,
		device.requiredFeatures = &float32filterable,
		device.requiredLimits = nullptr,
		device.defaultQueue = { .label = to_webgpu(config.queue_label) },
		device.uncapturedErrorCallbackInfo = {
			.callback = [](WGPUDevice const * device, WGPUErrorType type, WGPUStringView message, void* userdata1, void* userdata2) {
				get_error_handler()(stylizer::error_severity::Error, from_webgpu(message), (size_t)type);
			}
		};
		device.deviceLostCallbackInfo = {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPUDevice const * device, WGPUDeviceLostReason reason, WGPUStringView message, void* userdata1, void* userdata2) {
				if (reason == WGPUDeviceLostReason_Destroyed) return;
				STYLIZER_THROW(from_webgpu(message));
			}
		};
		wait_for_future(wgpuAdapterRequestDevice(out.adapter, &device, {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata, void*){
				WGPUDevice& out = *(WGPUDevice*)userdata;
				switch (status) {
				case WGPURequestDeviceStatus_CallbackCancelled: [[fallthrough]];
				case WGPURequestDeviceStatus_Error:
					STYLIZER_THROW(from_webgpu(message));
					[[fallthrough]];
				case WGPURequestDeviceStatus_Success: [[fallthrough]];
				case WGPURequestDeviceStatus_Force32:
					out = device;
				}
			},
			.userdata1 = &out.device_, .userdata2 = nullptr
		}));

		out.queue = wgpuDeviceGetQueue(out.device_);
		return out;
	}
	
}