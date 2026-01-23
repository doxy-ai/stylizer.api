module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:compute_pipeline;

import :device;
import :bind_group;

namespace stylizer::graphics::webgpu {

	export struct pipeline : public graphics::pipeline { };

	export struct compute_pipeline: public graphics::compute_pipeline { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(compute_pipeline); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(compute_pipeline);
		uint32_t type = magic_number;

		compute_pipeline(compute_pipeline&& o) { *this = std::move(o); }
		compute_pipeline& operator=(compute_pipeline&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static webgpu::compute_pipeline create(graphics::device& device, pipeline::entry_point entry_point, const std::string_view label = "Stylizer Compute Pipeline") { STYLIZER_THROW("Not implemented yet!"); }

		webgpu::bind_group create_bind_group(graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") { STYLIZER_THROW("Not implemented yet!"); }
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<compute_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(compute_pipeline_concept<compute_pipeline>);

	compute_pipeline device::create_compute_pipeline(const graphics::pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::compute_pipeline& device::create_compute_pipeline(graphics::temporary_return_t, const graphics::pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) { STYLIZER_THROW("Not implemented yet!"); }
}