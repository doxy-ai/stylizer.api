module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"
#include "../../util/defer.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:shader;

import :device;

namespace stylizer::graphics::webgpu {
    
	export struct shader : public graphics::shader { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(shader); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(shader);
		uint32_t type = magic_number;
		WGPUShaderModule module = nullptr;
		// graphics::spirv spirv = {}; // TODO: Can we store some sort of smaller reflection data?

		shader(shader&& o) { *this = std::move(o); }
		shader& operator=(shader&& o) {
			module = std::exchange(o.module, nullptr);
			// spirv = std::exchange(o.spirv, {});
			return *this;
		}
		inline operator bool() const override { return module; }

		static shader create_from_wgsl(graphics::device& device_, const std::string_view wgsl, const std::string_view label = "Stylizer Shader");

		static shader create_from_session(graphics::device& device, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");

		static shader create_from_spirv(graphics::device& device, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");

		static shader create_from_source(graphics::device& device, language lang, shader::stage stage, const std::string_view source, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");

		void release() override;
		stylizer::auto_release<shader> auto_release() { return std::move(*this); }
	};
	static_assert(shader_concept<shader>);
	
}