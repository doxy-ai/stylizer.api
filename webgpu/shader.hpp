#pragma once

#include "cstring_from_view.hpp"
#include "device.hpp"

namespace stylizer::api::webgpu {
	struct shader: public api::shader { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(shader);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::ShaderModule module = nullptr;
		slcross::spirv spirv; // TODO: Can we store some sort of smaller reflection data?

		shader(shader&& o) { *this = std::move(o); }
		shader& operator=(shader&& o) {
			module = std::exchange(o.module, nullptr);
			spirv = std::exchange(o.spirv, {});
			return *this;
		}
		inline operator bool() const override { return module; }

		static shader create_from_wgsl(api::device& device_, const std::string_view wgsl, const std::string_view label = "Stylizer Shader", std::optional<slcross::spirv> spirv = {}) {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			shader out;
			out.spirv = std::move(spirv.value_or(slcross::wgsl::parse_from_memory(wgsl)));

			WGPUShaderModuleWGSLDescriptor code {
				.chain = { .sType = wgpu::SType::ShaderModuleWGSLDescriptor },
				.code = cstring_from_view(wgsl),
			};
			WGPUShaderModuleDescriptor descriptor {
				.nextInChain = &code.chain,
				.label = cstring_from_view(label),
			};
			out.module = wgpuDeviceCreateShaderModule(device.device_, &descriptor);
			return out;
		}

		static shader create_from_spirv(api::device& device, slcross::spirv_view spirv, const std::string_view label = "Stylizer Shader") {
			return create_from_wgsl(device, slcross::wgsl::generate(spirv), label, slcross::spirv{spirv.begin(), spirv.end()});
		}

		inline static shader create_from_source(api::device& device, language lang, const std::string_view source, stage stage = shader_stage::Combined, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") {
			return api::shader::create_from_source<webgpu::shader>(device, lang, source, stage, entry_point, label);
		}

		void release() override {
			if(module) std::exchange(module, nullptr).release();
		}
	};
}