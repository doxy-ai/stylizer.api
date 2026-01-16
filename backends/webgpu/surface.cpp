#include "common.hpp"

#ifdef STYLIZER_API_SURFACE_SUPPORT_X11
#include <X11/Xlib.h>
#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_WAYLAND
#include <wayland-client-core.h>
#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_COCOA
#error "What headers do I need to include for cocoa support?"
#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_WIN32
#include <windows.h>
#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_EMSCRIPTEN
#include <cstring_from_view.hpp>
#endif

namespace stylizer::api::webgpu {

	inline surface::present_mode from_webgpu(WGPUPresentMode mode) {
		switch(mode) {
		case WGPUPresentMode_Fifo: return surface::present_mode::Fifo;
		case WGPUPresentMode_FifoRelaxed: return surface::present_mode::FifoRelaxed;
		case WGPUPresentMode_Immediate: return surface::present_mode::Immediate;
		case WGPUPresentMode_Mailbox: return surface::present_mode::Mailbox;
		default:
			STYLIZER_API_THROW(std::string("Failed to find present mode: ") + std::string(magic_enum::enum_name(mode)));
		}
		std::unreachable();
	}
	inline WGPUPresentMode to_webgpu(surface::present_mode mode) {
		switch(mode) {
		case surface::present_mode::Fifo: return WGPUPresentMode_Fifo;
		case surface::present_mode::FifoRelaxed: return WGPUPresentMode_FifoRelaxed;
		case surface::present_mode::Immediate: return WGPUPresentMode_Immediate;
		case surface::present_mode::Mailbox: return WGPUPresentMode_Mailbox;
		default:
			STYLIZER_API_THROW(std::string("Failed to find present mode: ") + std::string(magic_enum::enum_name(mode)));
		}
		std::unreachable();
	}

	inline alpha_mode from_webgpu(WGPUCompositeAlphaMode mode) {
		switch(mode) {
		case WGPUCompositeAlphaMode_Opaque: return alpha_mode::Opaque;
		case WGPUCompositeAlphaMode_Premultiplied: return alpha_mode::PreMultiplied;
		case WGPUCompositeAlphaMode_Unpremultiplied: return alpha_mode::PostMultiplied;
		case WGPUCompositeAlphaMode_Inherit: return alpha_mode::Inherit;
		default:
			STYLIZER_API_THROW(std::string("Failed to find alpha mode: ") + std::string(magic_enum::enum_name(mode)));
		}
		std::unreachable();
	}
	inline WGPUCompositeAlphaMode to_webgpu(alpha_mode mode) {
		switch(mode) {
		case alpha_mode::Opaque: return WGPUCompositeAlphaMode_Opaque;
		case alpha_mode::PostMultiplied: return WGPUCompositeAlphaMode_Premultiplied;
		case alpha_mode::PreMultiplied: return WGPUCompositeAlphaMode_Unpremultiplied;
		case alpha_mode::Inherit: return WGPUCompositeAlphaMode_Inherit;
		default:
			STYLIZER_API_THROW(std::string("Failed to find alpha mode: ") + std::string(magic_enum::enum_name(mode)));
		}
		std::unreachable();
	}

	surface surface::create_from_emscripten(const std::string_view canvas_id /* = "canvas" */) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_EMSCRIPTEN
		WGPUSurfaceSourceCanvasHTMLSelector_Emscripten fromCanvasHTMLSelector;
		fromCanvasHTMLSelector.chain.sType = WGPUSType_SurfaceSourceCanvasHTMLSelector_Emscripten;
		fromCanvasHTMLSelector.chain.next = NULL;
		fromCanvasHTMLSelector.selector = cstring_from_view(canvas_id);

		WGPUSurfaceDescriptor surfaceDescriptor;
		surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector.chain;
		surfaceDescriptor.label = NULL;

		struct surface out;
		out.surface_ = wgpuInstanceCreateSurface(get_common_instance(), &surfaceDescriptor);
		return out;
#else
		STYLIZER_API_THROW("Emscripten surfaces not supported!");
#endif
	}

	surface surface::create_from_cocoa(void* layer, void* window) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_COCOA
		id metal_layer = (id)layer;
		NSWindow* ns_window = (NSWindow*)window;

		WGPUSurfaceSourceMetalLayer fromMetalLayer;
		fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
		fromMetalLayer.chain.next = NULL;
		fromMetalLayer.layer = metal_layer;

		WGPUSurfaceDescriptor surfaceDescriptor;
		surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
		surfaceDescriptor.label = NULL;

		struct surface out;
		out.surface_ = wgpuInstanceCreateSurface(get_common_instance(), &surfaceDescriptor);
		return out;
#else
		STYLIZER_API_THROW("Cocoa surfaces not supported!");
#endif
	}

	surface surface::create_from_x11(void* display, void* window) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_X11
		Display* x11_display = (Display*)display;
		Window x11_window = (Window)window;

		WGPUSurfaceSourceXlibWindow fromXlibWindow;
		fromXlibWindow.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
		fromXlibWindow.chain.next = NULL;
		fromXlibWindow.display = x11_display;
		fromXlibWindow.window = x11_window;

		WGPUSurfaceDescriptor surfaceDescriptor;
		surfaceDescriptor.nextInChain = &fromXlibWindow.chain;
		surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

		surface out;
		out.surface_ = wgpuInstanceCreateSurface(get_common_instance(), &surfaceDescriptor);
		return out;
#else
		STYLIZER_API_THROW("X11 surfaces not supported!");
#endif
	}

	surface surface::create_from_wayland(void* display, void* surface) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_WAYLAND
		struct wl_display* wayland_display = (struct wl_display*)display;
		struct wl_surface* wayland_surface = (struct wl_surface*)surface;

		WGPUSurfaceSourceWaylandSurface fromWaylandSurface;
		fromWaylandSurface.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
		fromWaylandSurface.chain.next = NULL;
		fromWaylandSurface.display = wayland_display;
		fromWaylandSurface.surface = wayland_surface;

		WGPUSurfaceDescriptor surfaceDescriptor;
		surfaceDescriptor.nextInChain = &fromWaylandSurface.chain;
		surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

		struct surface out;
		out.surface_ = wgpuInstanceCreateSurface(get_common_instance(), &surfaceDescriptor);
		return out;
#else
		STYLIZER_API_THROW("Wayland surfaces not supported!");
#endif
	}

	surface surface::create_from_win32(void* window, void* instance) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_WIN32
		HWND hwnd = (HWND)window;
		HINSTANCE hinstance = (HINSTANCE)instance;

		WGPUSurfaceSourceWindowsHWND fromWindowsHWND;
		fromWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
		fromWindowsHWND.chain.next = NULL;
		fromWindowsHWND.hinstance = hinstance;
		fromWindowsHWND.hwnd = hwnd;

		WGPUSurfaceDescriptor surfaceDescriptor;
		surfaceDescriptor.nextInChain = &fromWindowsHWND.chain;
		surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

		struct surface out;
		out.surface_ = wgpuInstanceCreateSurface(get_common_instance(), &surfaceDescriptor);
		return out;
#else
		STYLIZER_API_THROW("Win32 surfaces not supported!");
#endif
	}

	surface::config surface::determine_optimal_default_config(api::device& device_, vec2u size) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		config out { .present_mode = present_mode::Fifo, .size = size };

		WGPUSurfaceCapabilities cap;
		wgpuSurfaceGetCapabilities(surface_, device.adapter, &cap);

		// Find the first SRGB surface format
		out.texture_format = texture_format::Undefined;
		for (size_t i = cap.formatCount; i--;)
			if (is_srgb(from_webgpu(cap.formats[i])))
				out.texture_format = from_webgpu(cap.formats[i]); // TODO: Write better choose logic!
		if (out.texture_format == texture_format::Undefined)
			out.texture_format = from_webgpu(cap.formats[0]);

		// Use the "Best" supported present mode
		if (auto end = cap.presentModes + cap.presentModeCount; std::find(cap.presentModes, end, WGPUPresentMode_Mailbox) != end)
			out.present_mode = present_mode::Mailbox;
		else if (std::find(cap.presentModes, end, WGPUPresentMode_Immediate) != end)
			out.present_mode = present_mode::Immediate;
		else if (std::find(cap.presentModes, end, WGPUPresentMode_FifoRelaxed) != end)
			out.present_mode = present_mode::FifoRelaxed;

		wgpuSurfaceCapabilitiesFreeMembers(cap);
		return out;
	}

	api::surface& surface::configure(api::device& device_, const config& config) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		WGPUSurfaceConfiguration c = WGPU_SURFACE_CONFIGURATION_INIT;
		c.device = device.device_,
		c.format = to_webgpu(configured_format = config.texture_format),
		c.usage = to_webgpu_texture(config.usage),
		c.alphaMode = to_webgpu(config.alpha_mode),
		c.width = static_cast<uint32_t>(config.size.x),
		c.height = static_cast<uint32_t>(config.size.y),
		c.presentMode = to_webgpu(config.present_mode),
		wgpuSurfaceConfigure(surface_, &c);
		return *this;
	}

#ifndef STYLIZER_NO_EXCEPTIONS
	#define STYLIZER_SURFACE_THROW(x) throw surface::texture_acquisition_failed(x)
#else
	#define STYLIZER_SURFACE_THROW(x) STYLIZER_API_THROW(x)
#endif

	texture surface::next_texture(api::device& device) {
		WGPUSurfaceTexture texture;
		wgpuSurfaceGetCurrentTexture(surface_, &texture);
		switch (texture.status) {
		case WGPUSurfaceGetCurrentTextureStatus_Timeout:
			STYLIZER_SURFACE_THROW("Failed to get next surface texture: " "Timed out");
		case WGPUSurfaceGetCurrentTextureStatus_Outdated:
			STYLIZER_SURFACE_THROW("Failed to get next surface texture: " "Outdated");
		case WGPUSurfaceGetCurrentTextureStatus_Lost:
			STYLIZER_SURFACE_THROW("Failed to get next surface texture: " "Current Texture Lost");
		case WGPUSurfaceGetCurrentTextureStatus_Error:
			STYLIZER_SURFACE_THROW("Failed to get next surface texture: " "Unknown Error");
		case WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal: [[fallthrough]]; // TODO: Should we do something in the case the success is suboptimal?
		case WGPUSurfaceGetCurrentTextureStatus_Force32: [[fallthrough]];
		case WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal:
			{ /* DO nothing */ }
		}

		webgpu::texture out;
		out.texture_ = texture.texture;
		return out;
	}
	api::texture& surface::next_texture(temporary_return_t, api::device& device) {
		static webgpu::texture out;
		return out = next_texture(device);
	}

	texture_format surface::configured_texture_format(api::device&) {
		return configured_format;
	}

	api::surface& surface::present(api::device& device) {
		wgpuSurfacePresent(surface_);
		return *this;
	}

	void surface::release() {
		if (surface_) wgpuSurfaceRelease(std::exchange(surface_, nullptr));
	}

	static_assert(surface_concept<surface>);
} // namespace stylizer::api::webgpu