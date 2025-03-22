#pragma once

#define STYLIZER_API_GLFW_AVAILABLE

#include "api.hpp"

// Modified from: https://github.com/eliemichel/glfw3webgpu/blob/main/glfw3webgpu.c

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#  define GLFW_EXPOSE_NATIVE_EMSCRIPTEN
#  ifndef GLFW_PLATFORM_EMSCRIPTEN // not defined in older versions of emscripten
#    define GLFW_PLATFORM_EMSCRIPTEN 0
#  endif
#else // __EMSCRIPTEN__
#  ifdef _GLFW_X11
#    define GLFW_EXPOSE_NATIVE_X11
#  endif
#  ifdef _GLFW_WAYLAND
#    define GLFW_EXPOSE_NATIVE_WAYLAND
#  endif
#  ifdef _GLFW_COCOA
#    define GLFW_EXPOSE_NATIVE_COCOA
#  endif
#  ifdef _GLFW_WIN32
#    define GLFW_EXPOSE_NATIVE_WIN32
#  endif
#endif // __EMSCRIPTEN__

#ifdef GLFW_EXPOSE_NATIVE_COCOA
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#endif

#ifndef __EMSCRIPTEN__
#  include <GLFW/glfw3native.h>
#endif

namespace stylizer::api::glfw {

	inline vec2u get_window_size(GLFWwindow* window) {
		int x, y;
		glfwGetWindowSize(window, &x, &y);
		return {static_cast<size_t>(x), static_cast<size_t>(y)};
	}

template<stylizer::api::surface_concept Tsurface>
Tsurface create_surface(GLFWwindow* window) {
#ifndef __EMSCRIPTEN__
	switch (glfwGetPlatform()) {
#else
	// glfwGetPlatform is not available in older versions of emscripten
	switch (GLFW_PLATFORM_EMSCRIPTEN) {
#endif

#ifdef GLFW_EXPOSE_NATIVE_X11
	case GLFW_PLATFORM_X11: {
		Display* x11_display = glfwGetX11Display();
		Window x11_window = glfwGetX11Window(window);

		return Tsurface::create_from_x11(x11_display, (void*)x11_window);
	}
#endif // GLFW_EXPOSE_NATIVE_X11

#ifdef GLFW_EXPOSE_NATIVE_WAYLAND
	case GLFW_PLATFORM_WAYLAND: {
		struct wl_display* wayland_display = glfwGetWaylandDisplay();
		struct wl_surface* wayland_surface = glfwGetWaylandWindow(window);

		return Tsurface::create_from_wayland(wayland_display, wayland_surface);
	}
#endif // GLFW_EXPOSE_NATIVE_WAYLAND

#ifdef GLFW_EXPOSE_NATIVE_COCOA
	case GLFW_PLATFORM_COCOA: {
		id metal_layer = [CAMetalLayer layer];
		NSWindow* ns_window = glfwGetCocoaWindow(window);
		[ns_window.contentView setWantsLayer : YES] ;
		[ns_window.contentView setLayer : metal_layer] ;

		return Tsurface::create_from_cocoa(metal_layer, ns_window);
	}
#endif // GLFW_EXPOSE_NATIVE_COCOA

#ifdef GLFW_EXPOSE_NATIVE_WIN32
	case GLFW_PLATFORM_WIN32: {
		HWND hwnd = glfwGetWin32Window(window);
		HINSTANCE hinstance = GetModuleHandle(NULL);

		return Tsurface::create_from_win32(hwnd, hinstance);
	}
#endif // GLFW_EXPOSE_NATIVE_WIN32

#ifdef GLFW_EXPOSE_NATIVE_EMSCRIPTEN
	case GLFW_PLATFORM_EMSCRIPTEN: {
		return Tsurface::create_from_emscripten("canvas");
	}
#endif // GLFW_EXPOSE_NATIVE_EMSCRIPTEN

	default:
		// Unsupported platform
		STYLIZER_API_THROW("Support has not been built for the current platform!");
	}
}

}