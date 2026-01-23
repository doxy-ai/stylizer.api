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

		static shader create_from_wgsl(graphics::device& device_, const std::string_view wgsl, const std::string_view label /* = "Stylizer Shader" */) {
			auto& device = confirm_webgpu_type<webgpu::device>(device_);

			shader out;
			// out.spirv = std::move(spirv.value_or(slcross::wgsl::parse_from_memory(wgsl)));

			WGPUShaderSourceWGSL code {
				.chain = { .sType = WGPUSType_ShaderSourceWGSL },
				.code = to_webgpu(wgsl),
			};
			WGPUShaderModuleDescriptor descriptor {
				.nextInChain = &code.chain,
				.label = to_webgpu(label),
			};
			out.module = wgpuDeviceCreateShaderModule(device.device_, &descriptor);
			return out;
		}

		static shader create_from_session(graphics::device& device, shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
			auto wgsl = slcross::wgsl::generate(session, entry_point);
			return create_from_wgsl(device, wgsl, label);
		}

		static shader create_from_spirv(graphics::device& device, shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
			auto session = slcross::experimental::spirv::parse_from_memory(spirv, {{to_slcross(stage), entry_point}});
			defer_ { session.free(); };
			auto wgsl = slcross::wgsl::generate(session, entry_point);
			return create_from_wgsl(device, wgsl, label);
		}

		static shader create_from_source(graphics::device& device, language lang, shader::stage stage, const std::string_view source, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
			if(lang == shader::language::WGSL) return create_from_wgsl(device, source, label);
			return graphics::shader::create_from_source<webgpu::shader>(device, lang, stage, source, entry_point, label);
		}

		void release() override {
			if (module) wgpuShaderModuleRelease(std::exchange(module, nullptr));
		}
		stylizer::auto_release<shader> auto_release() { return std::move(*this); }
	};
	static_assert(shader_concept<shader>);

	webgpu::shader device::create_shader_from_wgsl(const std::string_view wgsl, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_wgsl(*this, wgsl, label);
	}
	graphics::shader& device::create_shader_from_wgsl(temporary_return_t, const std::string_view wgsl, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_wgsl(wgsl, label);
	}

	webgpu::shader device::create_shader_from_session(shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_session(*this, stage, session, entry_point, label);
	}
	graphics::shader& device::create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_session(stage, session, entry_point, label);
	}

	webgpu::shader device::create_shader_from_spirv(shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_spirv(*this, stage, spirv, entry_point, label);
	}
	graphics::shader& device::create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_spirv(stage, spirv, entry_point, label);
	}

	webgpu::shader device::create_shader_from_source(shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_source(*this, lang, stage, source, entry_point.value_or("main"), label);
	}
	graphics::shader& device::create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_source(lang, stage, source, entry_point.value_or("main"), label);
	}
}