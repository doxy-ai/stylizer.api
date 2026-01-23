module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:command_encoder;

import :command_encoder_base;

namespace stylizer::graphics::webgpu {

	export struct command_encoder : public command_encoder_base<graphics::command_encoder, command_encoder> { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_encoder);
		using super = webgpu::command_encoder_base<graphics::command_encoder, command_encoder>;
		inline command_encoder(command_encoder&& o) { *this = std::move(o); }
		inline command_encoder& operator=(command_encoder&& o) { super::operator=(std::move(o)); return *this; }

		stylizer::auto_release<command_encoder> auto_release() { return std::move(*this); }
	};
	static_assert(command_encoder_concept<command_encoder>);

	webgpu::command_encoder device::create_command_encoder(bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		return webgpu::command_encoder::create(*this, one_shot, label);
	}

	graphics::command_encoder& device::create_command_encoder(temporary_return_t, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		static webgpu::command_encoder temp;
		return temp = create_command_encoder(one_shot, label);
	}	
}