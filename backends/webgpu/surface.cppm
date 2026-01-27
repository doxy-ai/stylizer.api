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
		texture::format configured_format = texture::format::Undefined;

		inline surface(surface&& o) { *this = std::move(o); }
		inline surface& operator=(surface&& o) {
			surface_ = std::exchange(o.surface_, nullptr);
			configured_format = std::exchange(o.configured_format, texture::format::Undefined);
			return *this;
		}
		inline operator bool() const override { return surface_; }

		static surface create_from_emscripten(const std::string_view canvas_id = "canvas");

		static surface create_from_cocoa(void* layer, void* window);

		static surface create_from_x11(void* display, void* window);

		static surface create_from_wayland(void* display, void* surface);

		static surface create_from_win32(void* window, void* instance);

		surface::config determine_optimal_default_config(graphics::device& device_, vec2u size) override;

		graphics::surface& configure(graphics::device& device_, const config& config) override;

		texture next_texture(graphics::device& device);
		graphics::texture& next_texture(temporary_return_t, graphics::device& device) override;

		texture_format configured_texture_format(graphics::device&) override;

		graphics::surface& present(graphics::device& device) override;

		void release() override;
		stylizer::auto_release<surface> auto_release() { return std::move(*this); }
	};
	static_assert(surface_concept<surface>);

}