module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:compute_pipeline;

import :device;
import :bind_group;
import :shader;

namespace stylizer::graphics::webgpu {

	export struct pipeline : public graphics::pipeline { };

	export struct compute_pipeline: public graphics::compute_pipeline { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(compute_pipeline); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(compute_pipeline);
		uint32_t type = magic_number;
		WGPUComputePipeline pipeline = nullptr;

		compute_pipeline(compute_pipeline&& o) { *this = std::move(o); }
		compute_pipeline& operator=(compute_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() const override { return pipeline; }

		static compute_pipeline create(graphics::device& device_, pipeline::entry_point entry_point, const std::string_view label = "Stylizer Compute Pipeline");

		webgpu::bind_group create_bind_group(graphics::device& device_, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group");
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") override;

		void release() override;
		stylizer::auto_release<compute_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(compute_pipeline_concept<compute_pipeline>);
}