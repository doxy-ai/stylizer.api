module;

// Modified from: https://github.com/eliemichel/sdl3webgpu/blob/main/sdl3webgpu.c

#include <SDL3/SDL.h>

#if defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_COCOA)
#  include <Cocoa/Cocoa.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#elif defined(SDL_PLATFORM_IOS)
#  include <UIKit/UIKit.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#  include <Metal/Metal.h>
#elif defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_WIN32)
#  include <windows.h>
#elif defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_X11)
#  include <X11/Xlib.h>
#elif defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_WAYLAND)
#  include <wayland-client-core.h>
#endif

export module stylizer.graphics.sdl3;

import std.compat;
import stylizer.errors;
import stylizer.graphics;

namespace stylizer::graphics::sdl3 {

	export template<stylizer::graphics::surface_concept Tsurface>
	Tsurface create_surface(SDL_Window* window) {
		SDL_PropertiesID props = SDL_GetWindowProperties(window);

#if defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_COCOA)
		{
			id metal_layer = NULL;
			NSWindow *ns_window = (__bridge NSWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
			if (!ns_window) return NULL;
			[ns_window.contentView setWantsLayer : YES];
			metal_layer = [CAMetalLayer layer];
			[ns_window.contentView setLayer : metal_layer];

			return Tsurface::create_from_cocoa(metal_layer, ns_window);
		}
#elif defined(SDL_PLATFORM_IOS)
		{
			UIWindow *ui_window = (__bridge UIWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
			if (!uiwindow) return NULL;

			UIView* ui_view = ui_window.rootViewController.view;
			CAMetalLayer* metal_layer = [CAMetalLayer new];
			metal_layer.opaque = true;
			metal_layer.frame = ui_view.frame;
			metal_layer.drawableSize = ui_view.frame.size;

			[ui_view.layer addSublayer: metal_layer];

			return Tsurface::create_from_cocoa(metal_layer, ui_window);
		}
#elif defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_X11) || defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_WAYLAND)
#  if defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_WAYLAND)
		if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
			struct wl_display *wayland_display = (struct wl_display *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
			struct wl_surface *wayland_surface = (struct wl_surface *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
			// if (!wayland_display || !wayland_surface) return NULL;

			return Tsurface::create_from_wayland(wayland_display, wayland_surface);
		}
#  endif // defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_WAYLAND)
#  if defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_X11)
		if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
			Display *x11_display = (Display *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
			Window x11_window = (Window)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
			// if (!x11_display || !x11_window) return NULL;

			return Tsurface::create_from_x11(x11_display, (void*)x11_window);
		}
#  endif // defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_X11)

#elif defined(STYLIZER_GRAPHICS_SURFACE_SUPPORT_WIN32)
		{
			HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
			// if (!hwnd) return NULL;
			HINSTANCE hinstance = GetModuleHandle(NULL);

			return Tsurface::create_from_win32(hwnd, hinstance);
		}
#elif defined(__EMSCRIPTEN__)
		{
			return Tsurface::create_from_emscripten("canvas");
		}  
#else
	// TODO: See SDL_syswm.h for other possible enum values!
	#error "Unsupported window system target"
#endif
		return {};
	}

	export template<stylizer::graphics::device_concept Tdevice, stylizer::graphics::surface_concept Tsurface>
	std::pair<Tdevice, Tsurface> create_surface_and_device(SDL_Window* window, graphics::device::create_config device_config = {}) {
		Tsurface surface = create_surface<Tsurface>(window);
		device_config.compatible_surface = &surface;
		Tdevice device = Tdevice::create_default(device_config);
		return {std::move(device), std::move(surface)};
	}
}