module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

export module stylizer.graphics.webgpu:shader;

import :device;

namespace stylizer::graphics::webgpu {
    
	export struct shader : public graphics::shader { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(shader); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(shader);
		uint32_t type = magic_number;

		shader(shader&& o) { *this = std::move(o); }
		shader& operator=(shader&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static webgpu::shader create_from_wgsl(graphics::device& device, const std::string_view wgsl, const std::string_view label = "Stylizer Shader") { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::shader create_from_session(graphics::device& device, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::shader create_from_spirv(graphics::device& device, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_THROW("Not implemented yet!"); }
		static webgpu::shader create_from_source(graphics::device& device, language lang, shader::stage stage, const std::string_view source, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<shader> auto_release() { return std::move(*this); }
	};
	static_assert(shader_concept<shader>);

	shader device::create_shader_from_session(graphics::shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::shader& device::create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) { STYLIZER_THROW("Not implemented yet!"); }
	shader device::create_shader_from_spirv(graphics::shader::stage stage, graphics::spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::shader& device::create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) { STYLIZER_THROW("Not implemented yet!"); }
	shader device::create_shader_from_source(graphics::shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::shader& device::create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) { STYLIZER_THROW("Not implemented yet!"); }

}