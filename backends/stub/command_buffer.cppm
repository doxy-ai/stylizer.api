module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:command_buffer;

import :device;

namespace stylizer::graphics::stub {
	export struct pipeline : public graphics::pipeline { };
	
	export struct command_buffer: public graphics::command_buffer { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_buffer); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_buffer);
		uint32_t type = magic_number;

		command_buffer(command_buffer&& o) { *this = std::move(o); }
		command_buffer& operator=(command_buffer&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		void submit(graphics::device& device, bool release = true) override { STYLIZER_THROW("Not implemented yet!"); }
		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<command_buffer> auto_release() { return std::move(*this); }
	};
}