module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:texture_view;

import :device;

namespace stylizer::graphics::webgpu {

	export struct texture_view : public graphics::texture_view { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(texture_view); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture_view);
		uint32_t type = magic_number;
		const webgpu::texture* owning_texture;
		WGPUTextureView view = nullptr;

		inline texture_view(texture_view&& o) { *this = std::move(o); }
		inline texture_view& operator=(texture_view&& o) {
			owning_texture = std::exchange(o.owning_texture, nullptr);
			view = std::exchange(o.view, nullptr);
			return *this;
		}
		inline operator bool() const override { return view; }

		static texture_view create(graphics::device& device, const graphics::texture& texture, const texture_view::create_config& config = {});

		const graphics::texture& texture() const override;

		void release() override {
			if (view) wgpuTextureViewRelease(std::exchange(view, nullptr));
		}
		stylizer::auto_release<texture_view> auto_release() { return std::move(*this); }
	};
	static_assert(texture_view_concept<texture_view>);

}