module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:texture_view;

import :device;

namespace stylizer::graphics::stub {
	
	export struct texture_view : public graphics::texture_view { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(texture_view); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture_view);
		uint32_t type = magic_number;

		inline texture_view(texture_view&& o) { *this = std::move(o); }
		inline texture_view& operator=(texture_view&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static stub::texture_view create(graphics::device& device, const graphics::texture& texture, const texture_view::create_config& config = {}) { STYLIZER_THROW("Not implemented yet!"); }

		const graphics::texture& texture() const override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<texture_view> auto_release() { return std::move(*this); }
	};
	static_assert(texture_view_concept<texture_view>);

}