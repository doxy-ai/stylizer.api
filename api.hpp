#pragma once

#include <concepts>
#include <cstddef>
#include <cassert>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <magic_enum/magic_enum.hpp>

namespace stylizer {
	template<typename T>
	concept releasable = requires(T t) {
		{t.release()};
	};

	template<typename T>
	struct auto_release: public T {
		auto_release() : T() {}
		auto_release(T&& raw) : T(std::move(raw)) {}
		auto_release(const auto_release&) = delete;
		auto_release(auto_release&& other) : T(std::move(other)) {}
		auto_release& operator=(T&& raw) { static_cast<T&>(*this) = std::move(raw); return *this; }
		auto_release& operator=(const auto_release&) = delete;
		auto_release& operator=(auto_release&& other) {{ static_cast<T&>(*this) = std::move(static_cast<T&>(other)); return *this; }}

		~auto_release() { if(*this) this->release(); }
	};

#define STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(type)\
		type() {}\
		type(const type&) = default;\
		type& operator=(const type&) = default;

#define STYLIZER_API_AS_METHOD(type) template<std::derived_from<type> T>\
	T& as() {\
		T* out = dynamic_cast<T*>(this);\
		assert(out != nullptr);\
		return *out;\
	}
}

namespace stylizer::api {
	using namespace magic_enum::bitwise_operators;

	struct error: public std::runtime_error { using std::runtime_error::runtime_error; };

	enum class alpha_mode {
		Opaque,
		PostMultiplied,
		PreMultiplied,
		Inherit,
	};

	enum class texture_format {
		RGBA8,
		RGBA8_SRGB,

		BGRA8_SRGB,
	};

	enum class usage { // TODO: Needs to be flags!
		Invalid = 0,
		RenderAttachment = (1 << 0),
	};

	struct vec2 { size_t x, y; };
	struct vec3 { size_t x, y, z; };
	struct color { float r, g, b, a; };

	struct device {
		struct create_config {
			bool high_performance = true;
			struct surface* surface = nullptr; // When set to null the device will be configured in headless mode
		};

		virtual void release(bool static_sub_objects = false) = 0;
		STYLIZER_API_AS_METHOD(device);
	};

	template<typename T>
	concept device_concept = requires(T t, device::create_config config) {
		{ T::create_default(config) } -> std::convertible_to<T>;
	};

	struct surface {
		enum class present_mode {
			Fifo = 0,
			FifoRelaxed,
			Immediate,
			Mailbox,
		};

		struct config {
			enum present_mode present_mode = surface::present_mode::Fifo;
			enum texture_format texture_format = texture_format::RGBA8_SRGB;
			enum alpha_mode alpha_mode = alpha_mode::Opaque;
			enum usage usage = usage::RenderAttachment;
			vec2 size;
		};
		virtual config determine_optimal_config(device& device, vec2 size) { config out{}; out.size = size; return out; }
		virtual surface& configure(device& device, config config) = 0;

		virtual surface& present(device& device) = 0;

		virtual void release() = 0;
		STYLIZER_API_AS_METHOD(surface);
	};

	template<typename T>
	concept surface_concept = requires(T t, void* display, void* surface, void* layer, void* instance, void* window, std::string_view canvas_id) {
		{ T::create_from_x11(display, window) } -> std::convertible_to<T>;
		{ T::create_from_wayland(display, surface) } -> std::convertible_to<T>;
		{ T::create_from_cocoa(layer, window) } -> std::convertible_to<T>;
		{ T::create_from_win32(window, instance) } -> std::convertible_to<T>;
		{ T::create_from_emscripten(canvas_id) } -> std::convertible_to<T>;
	};
}