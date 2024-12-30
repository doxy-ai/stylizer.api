#pragma once

#include "texture.hpp"

#ifdef STYLIZER_API_SURFACE_SUPPORT_X11
#include <X11/Xlib.h>
#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_WAYLAND

#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_COCOA

#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_WIN32

#endif

#ifdef STYLIZER_API_SURFACE_SUPPORT_EMSCRIPTEN
#include <cstring_from_view.hpp>
#endif

namespace stylizer::api::webgpu {
	struct surface: public api::surface { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(surface);
		uint32_t type = magic_number;
		wgpu::Surface surface_ = nullptr;

		inline surface(surface&& o) { *this = std::move(o); }
		inline surface& operator=(surface&& o) {
			surface_ = std::exchange(o.surface_, nullptr);
			return *this;
		}
		inline operator bool() const override { return surface_; }
		surface&& move() { return std::move(*this); }


		static surface create_from_emscripten(const std::string_view canvas_id = "canvas") {
#ifdef STYLIZER_API_SURFACE_SUPPORT_EMSCRIPTEN
#  ifdef WEBGPU_BACKEND_DAWN
			WGPUSurfaceSourceCanvasHTMLSelector_Emscripten fromCanvasHTMLSelector;
			fromCanvasHTMLSelector.chain.sType = WGPUSType_SurfaceSourceCanvasHTMLSelector_Emscripten;
#  else
			WGPUSurfaceDescriptorFromCanvasHTMLSelector fromCanvasHTMLSelector;
			fromCanvasHTMLSelector.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
#  endif
			fromCanvasHTMLSelector.chain.next = NULL;
			fromCanvasHTMLSelector.selector = cstring_from_view(canvas_id);

			WGPUSurfaceDescriptor surfaceDescriptor;
			surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector.chain;
			surfaceDescriptor.label = NULL;

			struct surface out;
			out.surface_ = wgpuInstanceCreateSurface(get_instance(), &surfaceDescriptor);
			return out;
#else
			STYLIZER_API_THROW("Emscripten surfaces not supported!");
#endif
		}

		static surface create_from_cocoa(void* layer, void* window) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_COCOA
			id metal_layer = (id)layer;
			NSWindow* ns_window = (NSWindow*)window;

#  ifdef WEBGPU_BACKEND_DAWN
			WGPUSurfaceSourceMetalLayer fromMetalLayer;
			fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
#  else
			WGPUSurfaceDescriptorFromMetalLayer fromMetalLayer;
			fromMetalLayer.chain.sType = WGPUSType_SurfaceDescriptorFromMetalLayer;
#  endif
			fromMetalLayer.chain.next = NULL;
			fromMetalLayer.layer = metal_layer;

			WGPUSurfaceDescriptor surfaceDescriptor;
			surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
			surfaceDescriptor.label = NULL;

			struct surface out;
			out.surface_ = wgpuInstanceCreateSurface(get_instance(), &surfaceDescriptor);
			return out;
#else
			STYLIZER_API_THROW("Cocoa surfaces not supported!");
#endif
		}

		static surface create_from_x11(void* display, void* window) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_X11
			Display* x11_display = (Display*)display;
			Window x11_window = (Window)window;

#  ifdef WEBGPU_BACKEND_DAWN
			WGPUSurfaceSourceXlibWindow fromXlibWindow;
			fromXlibWindow.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
#  else
			WGPUSurfaceDescriptorFromXlibWindow fromXlibWindow;
			fromXlibWindow.chain.sType = WGPUSType_SurfaceDescriptorFromXlibWindow;
#  endif
			fromXlibWindow.chain.next = NULL;
			fromXlibWindow.display = x11_display;
			fromXlibWindow.window = x11_window;

			WGPUSurfaceDescriptor surfaceDescriptor;
			surfaceDescriptor.nextInChain = &fromXlibWindow.chain;
			surfaceDescriptor.label = NULL;

			surface out;
			out.surface_ = wgpuInstanceCreateSurface(get_instance(), &surfaceDescriptor);
			return out;
#else
			STYLIZER_API_THROW("X11 surfaces not supported!");
#endif
		}

		static surface create_from_wayland(void* display, void* surface) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_WAYLAND
			struct wl_display* wayland_display = (struct wl_display*)display;
			struct wl_surface* wayland_surface = (struct wl_surface*)surface;

#  ifdef WEBGPU_BACKEND_DAWN
			WGPUSurfaceSourceWaylandSurface fromWaylandSurface;
			fromWaylandSurface.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
#  else
			WGPUSurfaceDescriptorFromWaylandSurface fromWaylandSurface;
			fromWaylandSurface.chain.sType = WGPUSType_SurfaceDescriptorFromWaylandSurface;
#  endif
			fromWaylandSurface.chain.next = NULL;
			fromWaylandSurface.display = wayland_display;
			fromWaylandSurface.surface = wayland_surface;

			WGPUSurfaceDescriptor surfaceDescriptor;
			surfaceDescriptor.nextInChain = &fromWaylandSurface.chain;
			surfaceDescriptor.label = NULL;

			struct surface out;
			out.surface_ = wgpuInstanceCreateSurface(get_instance(), &surfaceDescriptor);
			return out;
#else
			STYLIZER_API_THROW("Wayland surfaces not supported!");
#endif
		}

		static surface create_from_win32(void* window, void* instance) {
#ifdef STYLIZER_API_SURFACE_SUPPORT_WIN32
			HWND hwnd = (HWND)window;
			HINSTANCE hinstance = (HINSTANCE)instance;

#  ifdef WEBGPU_BACKEND_DAWN
			WGPUSurfaceSourceWindowsHWND fromWindowsHWND;
			fromWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
#  else
			WGPUSurfaceDescriptorFromWindowsHWND fromWindowsHWND;
			fromWindowsHWND.chain.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND;
#  endif
			fromWindowsHWND.chain.next = NULL;
			fromWindowsHWND.hinstance = hinstance;
			fromWindowsHWND.hwnd = hwnd;

			WGPUSurfaceDescriptor surfaceDescriptor;
			surfaceDescriptor.nextInChain = &fromWindowsHWND.chain;
			surfaceDescriptor.label = NULL;

			struct surface out;
			out.surface_ = wgpuInstanceCreateSurface(get_instance(), &surfaceDescriptor);
			return out;
#else
			STYLIZER_API_THROW("Win32 surfaces not supported!");
#endif
		}



		config determine_optimal_config(api::device& device_, vec2u size) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			config out{.present_mode = present_mode::Fifo, .size = size};

			out.texture_format = from_wgpu(surface_.getPreferredFormat(device.adapter));

			wgpu::SurfaceCapabilities cap;
			surface_.getCapabilities(device.adapter, &cap);
			if(auto end = cap.presentModes + cap.presentModeCount; std::find(cap.presentModes, end, WGPUPresentMode_Mailbox) != end)
				out.present_mode = present_mode::Mailbox;
			else if(std::find(cap.presentModes, end, WGPUPresentMode_Immediate) != end)
				out.present_mode = present_mode::Immediate;
			else if(std::find(cap.presentModes, end, WGPUPresentMode_FifoRelaxed) != end)
				out.present_mode = present_mode::FifoRelaxed;
			cap.freeMembers();

			return out;
		}

		api::surface& configure(api::device& device_, const config& config) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			surface_.configure(WGPUSurfaceConfiguration{
				.device = device.device_,
				.format = to_wgpu(config.texture_format),
				.usage = to_wgpu_texture(config.usage),
				.alphaMode = to_wgpu(config.alpha_mode),
				.width = static_cast<uint32_t>(config.size.x),
				.height = static_cast<uint32_t>(config.size.y),
				.presentMode = to_wgpu(config.present_mode),
			});
			return *this;
		}

		texture next_texture(api::device& device) {
			wgpu::SurfaceTexture texture;
			surface_.getCurrentTexture(&texture);
			switch(texture.status){
				case WGPUSurfaceGetCurrentTextureStatus_Force32:
				case WGPUSurfaceGetCurrentTextureStatus_Success: break;
				case WGPUSurfaceGetCurrentTextureStatus_Timeout:
#ifdef __cpp_exceptions
					throw texture_acquisition_failed("Failed to get next surface texture: " "Timed out");
#else
					assert(("Failed to get next surface texture: " "Timed out", false));
#endif
				case WGPUSurfaceGetCurrentTextureStatus_Outdated:
#ifdef __cpp_exceptions
					throw texture_acquisition_failed("Failed to get next surface texture: " "Outdated");
#else
					assert(("Failed to get next surface texture: " "Outdated", false));
#endif
				case WGPUSurfaceGetCurrentTextureStatus_Lost:
#ifdef __cpp_exceptions
					throw texture_acquisition_failed("Failed to get next surface texture: " "Texture Lost");
#else
					assert(("Failed to get next surface texture: " "Texture Lost", false));
#endif
				case WGPUSurfaceGetCurrentTextureStatus_OutOfMemory:
#ifdef __cpp_exceptions
					throw texture_acquisition_failed("Failed to get next surface texture: " "Out of memory");
#else
					assert(("Failed to get next surface texture: " "Out of memory", false));
#endif
				case WGPUSurfaceGetCurrentTextureStatus_DeviceLost:
#ifdef __cpp_exceptions
					throw texture_acquisition_failed("Failed to get next surface texture: " "Device Lost");
#else
					assert(("Failed to get next surface texture: " "Device Lost", false));
#endif
			}

			webgpu::texture out;
			out.texture_ = texture.texture;
			return out;
		}
		api::texture& next_texture(temporary_return_t, api::device& device) override {
			static texture out;
			return out = next_texture(device);
		}

		api::surface& present(api::device& device) override {
			surface_.present();
			return *this;
		}

		void release() override {
			if(surface_) std::exchange(surface_, nullptr).release();
		}
	};
	static_assert(surface_concept<surface>);
}