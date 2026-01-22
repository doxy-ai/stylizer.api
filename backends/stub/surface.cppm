module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:surface;

import :device;
import :texture;

namespace stylizer::graphics::stub {

	export struct surface : public graphics::surface { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(surface);  STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(surface);
		uint32_t type = magic_number;

		inline surface(surface&& o) { *this = std::move(o); }
		inline surface& operator=(surface&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static stub::surface create_from_emscripten(const std::string_view canvas_id = "canvas") { STYLIZER_THROW("Not implemented yet!"); }
		static stub::surface create_from_cocoa(void* layer, void* window) { STYLIZER_THROW("Not implemented yet!"); }
		static stub::surface create_from_x11(void* display, void* window) { STYLIZER_THROW("Not implemented yet!"); }
		static stub::surface create_from_wayland(void* display, void* surface) { STYLIZER_THROW("Not implemented yet!"); }
		static stub::surface create_from_win32(void* window, void* instance) { STYLIZER_THROW("Not implemented yet!"); }

		config determine_optimal_default_config(graphics::device& device, vec2u size) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::surface& configure(graphics::device& device, const config& config) override { STYLIZER_THROW("Not implemented yet!"); }

		stub::texture next_texture(graphics::device& device) { STYLIZER_THROW("Not implemented yet!"); }
		graphics::texture& next_texture(temporary_return_t, graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		texture::format configured_texture_format(graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::surface& present(graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<surface> auto_release() { return std::move(*this); }
	};
	static_assert(surface_concept<surface>);
	
}