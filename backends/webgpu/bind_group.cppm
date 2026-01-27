module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:bind_group;

import :device;
import :buffer;
import :texture;

namespace stylizer::graphics::webgpu {

	export struct bind_group: public graphics::bind_group { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(bind_group); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(bind_group);
		uint32_t type = magic_number;
		WGPUBindGroup group = nullptr;
		size_t index = 0;

		bind_group(bind_group&& o) { *this = std::move(o); }
		bind_group& operator=(bind_group&& o) {
			group = std::exchange(o.group, nullptr);
			index = o.index;
			return *this;
		}
		inline operator bool() const override { return group; }

		static bind_group internal_create(webgpu::device& device, size_t index, WGPUBindGroupLayout layout, std::span<const binding> bindings, std::string_view label);

		void release() override;
		stylizer::auto_release<bind_group> auto_release() { return std::move(*this); }
	};
}