#include "texture.hpp"
#include "command_encoder.hpp"
#include "common.hpp"
#include "shader.hpp"
#include "compute_pipeline.hpp"
#include <cstdint>

namespace stylizer::api::webgpu {
	std::string format_to_string(WGPUTextureFormat format) {
		switch(static_cast<wgpu::TextureFormat>(format)) {
			case WGPUTextureFormat_Undefined: return "undefined";
			case WGPUTextureFormat_R8Unorm: return "r8unorm";
			case WGPUTextureFormat_R8Snorm: return "r8snorm";
			case WGPUTextureFormat_R8Uint: return "r8uint";
			case WGPUTextureFormat_R8Sint: return "r8sint";
			case WGPUTextureFormat_R16Uint: return "r16uint";
			case WGPUTextureFormat_R16Sint: return "r16sint";
			case WGPUTextureFormat_R16Float: return "r16float";
			case WGPUTextureFormat_RG8Unorm: return "rg8unorm";
			case WGPUTextureFormat_RG8Snorm: return "rg8snorm";
			case WGPUTextureFormat_RG8Uint: return "rg8uint";
			case WGPUTextureFormat_RG8Sint: return "rg8sint";
			case WGPUTextureFormat_R32Float: return "r32float";
			case WGPUTextureFormat_R32Uint: return "r32uint";
			case WGPUTextureFormat_R32Sint: return "r32sint";
			case WGPUTextureFormat_RG16Uint: return "rg16uint";
			case WGPUTextureFormat_RG16Sint: return "rg16sint";
			case WGPUTextureFormat_RG16Float: return "rg16float";
			case WGPUTextureFormat_RGBA8Unorm: return "rgba8unorm";
			case WGPUTextureFormat_RGBA8UnormSrgb: return "rgba8unorm";
			case WGPUTextureFormat_RGBA8Snorm: return "rgba8snorm";
			case WGPUTextureFormat_RGBA8Uint: return "rgba8uint";
			case WGPUTextureFormat_RGBA8Sint: return "rgba8sint";
			case WGPUTextureFormat_BGRA8Unorm: return "bgra8unorm";
			case WGPUTextureFormat_BGRA8UnormSrgb: return "bgra8unorm";
			case WGPUTextureFormat_RGB10A2Uint: return "rgb10a2uint";
			case WGPUTextureFormat_RGB10A2Unorm: return "rgb10a2unorm";
			case WGPUTextureFormat_RG11B10Ufloat: return "rg11b10ufloat";
			case WGPUTextureFormat_RGB9E5Ufloat: return "rgb9e5ufloat";
			case WGPUTextureFormat_RG32Float: return "rg32float";
			case WGPUTextureFormat_RG32Uint: return "rg32uint";
			case WGPUTextureFormat_RG32Sint: return "rg32sint";
			case WGPUTextureFormat_RGBA16Uint: return "rgba16uint";
			case WGPUTextureFormat_RGBA16Sint: return "rgba16sint";
			case WGPUTextureFormat_RGBA16Float: return "rgba16float";
			case WGPUTextureFormat_RGBA32Float: return "rgba32float";
			case WGPUTextureFormat_RGBA32Uint: return "rgba32uint";
			case WGPUTextureFormat_RGBA32Sint: return "rgba32sint";
			case WGPUTextureFormat_Stencil8: return "stencil8";
			case WGPUTextureFormat_Depth16Unorm: return "depth16unorm";
			case WGPUTextureFormat_Depth24Plus: return "depth24plus";
			case WGPUTextureFormat_Depth24PlusStencil8: return "depth24plusstencil8";
			case WGPUTextureFormat_Depth32Float: return "depth32float";
			case WGPUTextureFormat_Depth32FloatStencil8: return "depth32floatstencil8";
			case WGPUTextureFormat_BC1RGBAUnorm: return "bc1rgbaunorm";
			case WGPUTextureFormat_BC1RGBAUnormSrgb: return "bc1rgbaunorm";
			case WGPUTextureFormat_BC2RGBAUnorm: return "bc2rgbaunorm";
			case WGPUTextureFormat_BC2RGBAUnormSrgb: return "bc2rgbaunorm";
			case WGPUTextureFormat_BC3RGBAUnorm: return "bc3rgbaunorm";
			case WGPUTextureFormat_BC3RGBAUnormSrgb: return "bc3rgbaunorm";
			case WGPUTextureFormat_BC4RUnorm: return "bc4runorm";
			case WGPUTextureFormat_BC4RSnorm: return "bc4rsnorm";
			case WGPUTextureFormat_BC5RGUnorm: return "bc5rgunorm";
			case WGPUTextureFormat_BC5RGSnorm: return "bc5rgsnorm";
			case WGPUTextureFormat_BC6HRGBUfloat: return "bc6hrgbufloat";
			case WGPUTextureFormat_BC6HRGBFloat: return "bc6hrgbfloat";
			case WGPUTextureFormat_BC7RGBAUnorm: return "bc7rgbaunorm";
			case WGPUTextureFormat_BC7RGBAUnormSrgb: return "bc7rgbaunorm";
			case WGPUTextureFormat_ETC2RGB8Unorm: return "etc2rgb8unorm";
			case WGPUTextureFormat_ETC2RGB8UnormSrgb: return "etc2rgb8unorm";
			case WGPUTextureFormat_ETC2RGB8A1Unorm: return "etc2rgb8a1unorm";
			case WGPUTextureFormat_ETC2RGB8A1UnormSrgb: return "etc2rgb8a1unorm";
			case WGPUTextureFormat_ETC2RGBA8Unorm: return "etc2rgba8unorm";
			case WGPUTextureFormat_ETC2RGBA8UnormSrgb: return "etc2rgba8unorm";
			case WGPUTextureFormat_EACR11Unorm: return "eacr11unorm";
			case WGPUTextureFormat_EACR11Snorm: return "eacr11snorm";
			case WGPUTextureFormat_EACRG11Unorm: return "eacrg11unorm";
			case WGPUTextureFormat_EACRG11Snorm: return "eacrg11snorm";
			case WGPUTextureFormat_ASTC4x4Unorm: return "astc4x4unorm";
			case WGPUTextureFormat_ASTC4x4UnormSrgb: return "astc4x4unorm";
			case WGPUTextureFormat_ASTC5x4Unorm: return "astc5x4unorm";
			case WGPUTextureFormat_ASTC5x4UnormSrgb: return "astc5x4unorm";
			case WGPUTextureFormat_ASTC5x5Unorm: return "astc5x5unorm";
			case WGPUTextureFormat_ASTC5x5UnormSrgb: return "astc5x5unorm";
			case WGPUTextureFormat_ASTC6x5Unorm: return "astc6x5unorm";
			case WGPUTextureFormat_ASTC6x5UnormSrgb: return "astc6x5unorm";
			case WGPUTextureFormat_ASTC6x6Unorm: return "astc6x6unorm";
			case WGPUTextureFormat_ASTC6x6UnormSrgb: return "astc6x6unorm";
			case WGPUTextureFormat_ASTC8x5Unorm: return "astc8x5unorm";
			case WGPUTextureFormat_ASTC8x5UnormSrgb: return "astc8x5unorm";
			case WGPUTextureFormat_ASTC8x6Unorm: return "astc8x6unorm";
			case WGPUTextureFormat_ASTC8x6UnormSrgb: return "astc8x6unorm";
			case WGPUTextureFormat_ASTC8x8Unorm: return "astc8x8unorm";
			case WGPUTextureFormat_ASTC8x8UnormSrgb: return "astc8x8unorm";
			case WGPUTextureFormat_ASTC10x5Unorm: return "astc10x5unorm";
			case WGPUTextureFormat_ASTC10x5UnormSrgb: return "astc10x5unorm";
			case WGPUTextureFormat_ASTC10x6Unorm: return "astc10x6unorm";
			case WGPUTextureFormat_ASTC10x6UnormSrgb: return "astc10x6unorm";
			case WGPUTextureFormat_ASTC10x8Unorm: return "astc10x8unorm";
			case WGPUTextureFormat_ASTC10x8UnormSrgb: return "astc10x8unorm";
			case WGPUTextureFormat_ASTC10x10Unorm: return "astc10x10unorm";
			case WGPUTextureFormat_ASTC10x10UnormSrgb: return "astc10x10unorm";
			case WGPUTextureFormat_ASTC12x10Unorm: return "astc12x10unorm";
			case WGPUTextureFormat_ASTC12x10UnormSrgb: return "astc12x10unorm";
			case WGPUTextureFormat_ASTC12x12Unorm: return "astc12x12unorm";
			case WGPUTextureFormat_ASTC12x12UnormSrgb: return "astc12x12unorm";
			case WGPUTextureFormat_Force32: return "force32";
			default: return 0;
		}
	}

	api::texture& texture::generate_mipmaps(api::device& device_, size_t first_mip_level /* = 0 */, std::optional<size_t> mip_levels_override /* = {} */) {
		auto& device = confirm_wgpu_type<webgpu::device>(device_);
		auto size = this->size();
		size_t size_max_levels = std::bit_width(std::min(size.x, size.y));
		uint32_t mip_levels = first_mip_level + mip_levels_override.value_or(size_max_levels - first_mip_level);
		assert(mip_levels <= size_max_levels);

		auto formatStr = format_to_string(texture_.getFormat());
		auto_release mipShader = webgpu::shader::create_from_wgsl(device, R"_(
@group(0) @binding(0) var previousMipLevel: texture_2d<f32>;
@group(0) @binding(1) var nextMipLevel: texture_storage_2d<)_" + formatStr + R"_(, write>;

@compute @workgroup_size(8, 8)
fn compute(@builtin(global_invocation_id) id: vec3<u32>) {
	let offset = vec2<u32>(0, 1);
	let color = (
		textureLoad(previousMipLevel, 2 * id.xy + offset.xx, 0) +
		textureLoad(previousMipLevel, 2 * id.xy + offset.xy, 0) +
		textureLoad(previousMipLevel, 2 * id.xy + offset.yx, 0) +
		textureLoad(previousMipLevel, 2 * id.xy + offset.yy, 0)
	) * 0.25;
	textureStore(nextMipLevel, id.xy, color);
})_");

		auto_release new_texture = device.create_texture({.format = this->texture_format(), .usage = usage() | usage::CopyDestination | usage::Storage, .size = size, .mip_levels = mip_levels, .samples = samples()});
		new_texture.copy_from(device, *this, {}, {}, {}, 0, 1);

		auto_release pipeline = device.create_compute_pipeline({&mipShader, "compute"}, "Stylizer Mipmapping Pipeline");
		auto encoder = device.create_command_encoder(true);
		encoder.bind_compute_pipeline(device, pipeline);

		// wgpu::Extent3D mipLevelSize = {size.x, size.y, 1}; // TODO: do we need a tweak to properly handle cubemaps?
		for (uint32_t level = 1; level < mip_levels; ++level) {
			vec3u invocationCount = {size.x / 2, size.y / 2, 1};
			constexpr uint32_t workgroupSizePerDim = 8;

			auto previous = new_texture.create_view(device, {.base_mip_level = level - 1, .mip_level_count_override = 1});
			auto current = new_texture.create_view(device, {.base_mip_level = level, .mip_level_count_override = 1});

			vec3u workgroups = {(invocationCount.x + workgroupSizePerDim + 1) / workgroupSizePerDim, (invocationCount.y + workgroupSizePerDim + 1) / workgroupSizePerDim, 1};
			encoder.bind_compute_group(device, pipeline.create_bind_group(device, 0, std::array<bind_group::binding, 2>{
					bind_group::texture_binding{.texture_view = &previous, .sampled_override = false}, bind_group::texture_binding{.texture_view = &current, .sampled_override = false}
				}), true)
				.dispatch_workgroups(device, workgroups);

			size = invocationCount;
			encoder.defer([view = std::move(previous)]() mutable { view.release(); });
			encoder.defer([view = std::move(current)]() mutable { view.release(); });
		}
		encoder.one_shot_submit(device);

		new_texture.sampler = std::exchange(sampler, nullptr);

		this->release();
		return (*this) = std::move(new_texture);
	}
}