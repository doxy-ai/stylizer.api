#pragma once

#include "device.hpp"

namespace stylizer::api::webgpu {
	struct texture_view: public api::texture_view { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(texture_view);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		const webgpu::texture* owning_texture;
		wgpu::TextureView view = nullptr;

		inline texture_view(texture_view&& o) { *this = std::move(o); }
		inline texture_view& operator=(texture_view&& o) {
			owning_texture = std::exchange(o.owning_texture, nullptr);
			view = std::exchange(o.view, nullptr);
			return *this;
		}
		inline operator bool() const override { return view; }
		texture_view&& move() { return std::move(*this); }

		static texture_view create(api::device& device, const api::texture& texture, const texture_view::create_config& config = {});

		const api::texture& texture() const override;

		void release() override {
			if(view) std::exchange(view, nullptr).release();
		}
	};
	static_assert(texture_view_concept<texture_view>);
}