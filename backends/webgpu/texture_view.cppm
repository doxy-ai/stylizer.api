module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:texture_view;

import :device;

namespace stylizer::graphics::webgpu {
	
	inline enum texture_view::create_config::aspect from_webgpu(WGPUTextureAspect aspect){
		switch(aspect){
		case WGPUTextureAspect_All: return texture_view::create_config::aspect::All;
		case WGPUTextureAspect_DepthOnly: return texture_view::create_config::aspect::DepthOnly;
		case WGPUTextureAspect_StencilOnly: return texture_view::create_config::aspect::StencilOnly;
		default:
			STYLIZER_THROW(std::string("Failed to find texture aspect: ") + std::string(magic_enum::enum_name(aspect)));
		}
		std::unreachable();
	}

	inline WGPUTextureAspect to_webgpu(enum texture_view::create_config::aspect aspect){
		switch(aspect){
		case texture_view::create_config::aspect::All: return WGPUTextureAspect_All;
		case texture_view::create_config::aspect::DepthOnly: return WGPUTextureAspect_DepthOnly;
		case texture_view::create_config::aspect::StencilOnly: return WGPUTextureAspect_StencilOnly;
		}
		std::unreachable();
	}

	struct texture;

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