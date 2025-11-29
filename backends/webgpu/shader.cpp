#include "common.hpp"

namespace stylizer::api::webgpu {

	shader shader::create_from_wgsl(api::device& device_, const std::string_view wgsl, const std::string_view label /* = "Stylizer Shader" */) {
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

	shader shader::create_from_session(api::device& device, shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		auto wgsl = slcross::wgsl::generate(session, entry_point);
		return create_from_wgsl(device, wgsl, label);
	}

	shader shader::create_from_spirv(api::device& device, shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		auto session = slcross::experimental::spirv::parse_from_memory(spirv, {{to_slcross(stage), entry_point}});
		defer_ { session.free(); };
		auto wgsl = slcross::wgsl::generate(session, entry_point);
		return create_from_wgsl(device, wgsl, label);
	}

	shader shader::create_from_source(api::device& device, language lang, shader::stage stage, const std::string_view source, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		if(lang == shader::language::WGSL) return create_from_wgsl(device, source, label);
		return api::shader::create_from_source<webgpu::shader>(device, lang, stage, source, entry_point, label);
	}

	void shader::release() {
		if (module) wgpuShaderModuleRelease(std::exchange(module, nullptr));
	}

	static_assert(shader_concept<shader>);
} // namespace stylizer::api::webgpu