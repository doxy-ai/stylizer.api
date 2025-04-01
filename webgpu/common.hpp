#pragma once

#include "../api.hpp"

#ifdef STYLIZER_API_IMPLEMENTATION
#define WEBGPU_CPP_IMPLEMENTATION
#endif
#include <webgpu/webgpu.hpp>

#ifdef WEBGPU_BACKEND_WGPU
#include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU

#include <algorithm>
#include "cstring_from_view.hpp"

namespace stylizer::api::webgpu {

	#define STYLIZER_API_WGPU_MAGIC_NUMBER 0x57475055/*"WGPU"*/
	constexpr static const uint32_t magic_number = STYLIZER_API_WGPU_MAGIC_NUMBER;

	template<typename Tto, typename Tfrom>
	inline Tto& confirm_wgpu_type(Tfrom& ref) {
		auto& res = ref.template as<Tto>();
		assert(res.type == magic_number);
		return res;
	}
	template<typename Tto, typename Tfrom>
	inline const Tto& confirm_wgpu_type(const Tfrom& ref) {
		auto& res = const_cast<Tfrom&>(ref).template as<Tto>();
		assert(res.type == magic_number);
		return res;
	}

	inline wgpu::Instance get_instance() {
		static auto_release<wgpu::Instance> instance = wgpu::createInstance({});
		return instance;
	}

	constexpr static std::string_view fullscreen_triangle_vertex_shader = R"_(
struct vertex_output {
	@builtin(position) raw_position: vec4f,
	@location(0) uv: vec2f
};

@group(0) @binding(0) var texture: texture_2d<f32>;
@group(0) @binding(1) var sampler_: sampler;

@vertex
fn vertex(@builtin(vertex_index) vertex_index: u32) -> vertex_output {
	switch vertex_index {
		case 0u: {
			return vertex_output(vec4f(-1.0, -3.0, .99, 1.0), vec2f(0.0, 2.0));
		}
		case 1u: {
			return vertex_output(vec4f(3.0, 1.0, .99, 1.0), vec2f(2.0, 0.0));
		}
		case 2u, default: {
			return vertex_output(vec4f(-1.0, 1.0, .99, 1.0), vec2f(0.0));
		}
	}
}
)_";

	inline wgpu::OptionalBool to_wgpu(bool boolean) {
		return boolean ? wgpu::OptionalBool::True : wgpu::OptionalBool::False;
	}

	inline wgpu::Color to_wgpu(color32 color) {
		return {color.r, color.g, color.b, color.a};
	}

	inline wgpu::CompositeAlphaMode to_wgpu(alpha_mode mode) {
		switch(mode){
			case alpha_mode::Opaque: return wgpu::CompositeAlphaMode::Opaque;
			case alpha_mode::PostMultiplied: return wgpu::CompositeAlphaMode::Unpremultiplied;
			case alpha_mode::PreMultiplied: return wgpu::CompositeAlphaMode::Premultiplied;
			case alpha_mode::Inherit: return wgpu::CompositeAlphaMode::Inherit;
		}
		STYLIZER_API_THROW("Unknown Alpha Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::TextureAspect to_wgpu(enum texture_view::create_config::aspect aspect) {
		switch(aspect){
		case texture_view::create_config::aspect::All: return wgpu::TextureAspect::All;
		case texture_view::create_config::aspect::DepthOnly: return wgpu::TextureAspect::DepthOnly;
		case texture_view::create_config::aspect::StencilOnly: return wgpu::TextureAspect::StencilOnly;
		}
		STYLIZER_API_THROW("Unknown Texture Aspect: " + std::string(magic_enum::enum_name(aspect)));
	}

	inline enum texture_view::create_config::aspect default_aspect(texture_format format) {
		switch(format) {
		case texture_format::Depth24: return texture_view::create_config::aspect::DepthOnly;
		default: return texture_view::create_config::aspect::All;
		}
	};

	inline bool is_srgb(wgpu::TextureFormat format) {
		switch(format) {
			case WGPUTextureFormat_RGBA8UnormSrgb:    
			case WGPUTextureFormat_BGRA8UnormSrgb:  
			case WGPUTextureFormat_BC1RGBAUnormSrgb:
			case WGPUTextureFormat_BC2RGBAUnormSrgb:
			case WGPUTextureFormat_BC3RGBAUnormSrgb:
			case WGPUTextureFormat_BC7RGBAUnormSrgb:                
			case WGPUTextureFormat_ETC2RGB8UnormSrgb:                
			case WGPUTextureFormat_ETC2RGB8A1UnormSrgb:
			case WGPUTextureFormat_ETC2RGBA8UnormSrgb:
			case WGPUTextureFormat_ASTC4x4UnormSrgb:
			case WGPUTextureFormat_ASTC5x4UnormSrgb:
			case WGPUTextureFormat_ASTC5x5UnormSrgb:
			case WGPUTextureFormat_ASTC6x5UnormSrgb:
			case WGPUTextureFormat_ASTC6x6UnormSrgb:
			case WGPUTextureFormat_ASTC8x5UnormSrgb:
			case WGPUTextureFormat_ASTC8x6UnormSrgb:
			case WGPUTextureFormat_ASTC8x8UnormSrgb:
			case WGPUTextureFormat_ASTC10x5UnormSrgb:
			case WGPUTextureFormat_ASTC10x6UnormSrgb:
			case WGPUTextureFormat_ASTC10x8UnormSrgb:
			case WGPUTextureFormat_ASTC10x10UnormSrgb:
			case WGPUTextureFormat_ASTC12x10UnormSrgb:
			case WGPUTextureFormat_ASTC12x12UnormSrgb:
				return true;
			default:
				return false;
		};
    }

	inline wgpu::TextureFormat to_wgpu(texture_format format) {
		switch(format){
			case texture_format::Undefined: return wgpu::TextureFormat::Undefined;
			case texture_format::Ru8_Normalized: return wgpu::TextureFormat::R8Unorm;
			case texture_format::Ri8_Normalized: return wgpu::TextureFormat::R8Snorm;
			case texture_format::Ru8: return wgpu::TextureFormat::R8Uint;
			case texture_format::Ri8: return wgpu::TextureFormat::R8Sint;
			case texture_format::Ru16: return wgpu::TextureFormat::R16Uint;
			case texture_format::Ri16: return wgpu::TextureFormat::R16Sint;
			case texture_format::Rf16: return wgpu::TextureFormat::R16Float;
			case texture_format::RGu8_Normalized: return wgpu::TextureFormat::RG8Unorm;
			case texture_format::RGi8_Normalized: return wgpu::TextureFormat::RG8Snorm;
			case texture_format::RGu8: return wgpu::TextureFormat::RG8Uint;
			case texture_format::RGi8: return wgpu::TextureFormat::RG8Sint;
			case texture_format::Rf32: return wgpu::TextureFormat::R32Float;
			case texture_format::Ru32: return wgpu::TextureFormat::R32Uint;
			case texture_format::Ri32: return wgpu::TextureFormat::R32Sint;
			case texture_format::RGu16: return wgpu::TextureFormat::RG16Uint;
			case texture_format::RGi16: return wgpu::TextureFormat::RG16Sint;
			case texture_format::RGf16: return wgpu::TextureFormat::RG16Float;
			case texture_format::RGBAu8_Normalized: return wgpu::TextureFormat::RGBA8Unorm;
			case texture_format::RGBAu8_NormalizedSRGB: return wgpu::TextureFormat::RGBA8UnormSrgb;
			case texture_format::RGBAi8_Normalized: return wgpu::TextureFormat::RGBA8Snorm;
			case texture_format::RGBAu8: return wgpu::TextureFormat::RGBA8Uint;
			case texture_format::RGBAi8: return wgpu::TextureFormat::RGBA8Sint;
			case texture_format::BGRAu8_Normalized: return wgpu::TextureFormat::BGRA8Unorm;
			case texture_format::BGRAu8_NormalizedSRGB: return wgpu::TextureFormat::BGRA8UnormSrgb;
			case texture_format::RGBu10Au2: return wgpu::TextureFormat::RGB10A2Uint;
			case texture_format::RGBu10Au2_Normalized: return wgpu::TextureFormat::RGB10A2Unorm;
			case texture_format::RGf11Bf10: return wgpu::TextureFormat::RG11B10Ufloat;
			case texture_format::RGf32: return wgpu::TextureFormat::RG32Float;
			case texture_format::RGu32: return wgpu::TextureFormat::RG32Uint;
			case texture_format::RGi32: return wgpu::TextureFormat::RG32Sint;
			case texture_format::RGBAu16: return wgpu::TextureFormat::RGBA16Uint;
			case texture_format::RGBAi16: return wgpu::TextureFormat::RGBA16Sint;
			case texture_format::RGBAf16: return wgpu::TextureFormat::RGBA16Float;
			case texture_format::RGBAf32: return wgpu::TextureFormat::RGBA32Float;
			case texture_format::RGBAu32: return wgpu::TextureFormat::RGBA32Uint;
			case texture_format::RGBAi32: return wgpu::TextureFormat::RGBA32Sint;
			case texture_format::Stencil_u8: return wgpu::TextureFormat::Stencil8;
			case texture_format::Depth_u16: return wgpu::TextureFormat::Depth16Unorm;
			case texture_format::Depth_u24: return wgpu::TextureFormat::Depth24Plus;
			case texture_format::Depth_u24Stencil_u8: return wgpu::TextureFormat::Depth24PlusStencil8;
			case texture_format::Depth_f32: return wgpu::TextureFormat::Depth32Float;
			case texture_format::Depth_f32Stencil_u8: return wgpu::TextureFormat::Depth32FloatStencil8;
			case texture_format::BC1RGBA_Normalized: return wgpu::TextureFormat::BC1RGBAUnorm;
			case texture_format::BC1RGBA_NormalizedSRGB: return wgpu::TextureFormat::BC1RGBAUnormSrgb;
			case texture_format::BC2RGBA_Normalized: return wgpu::TextureFormat::BC2RGBAUnorm;
			case texture_format::BC2RGBA_NormalizedSRGB: return wgpu::TextureFormat::BC2RGBAUnormSrgb;
			case texture_format::BC3RGBA_Normalized: return wgpu::TextureFormat::BC3RGBAUnorm;
			case texture_format::BC3RGBA_NormalizedSRGB: return wgpu::TextureFormat::BC3RGBAUnormSrgb;
			case texture_format::BC7RGBA_Normalized: return wgpu::TextureFormat::BC7RGBAUnorm;
			case texture_format::BC7RGBA_NormalizedSRGB: return wgpu::TextureFormat::BC7RGBAUnormSrgb;
			case texture_format::ETC2RGB8_Normalized: return wgpu::TextureFormat::ETC2RGB8Unorm;
			case texture_format::ETC2RGB8_NormalizedSRGB: return wgpu::TextureFormat::ETC2RGB8UnormSrgb;
			case texture_format::ETC2RGB8A1_Normalized: return wgpu::TextureFormat::ETC2RGB8A1Unorm;
			case texture_format::ETC2RGB8A1_NormalizedSRGB: return wgpu::TextureFormat::ETC2RGB8A1UnormSrgb;
			case texture_format::ETC2RGBA8_Normalized: return wgpu::TextureFormat::ETC2RGBA8Unorm;
			case texture_format::ETC2RGBA8_NormalizedSRGB: return wgpu::TextureFormat::ETC2RGBA8UnormSrgb;
			case texture_format::EACRu11_Normalized: return wgpu::TextureFormat::EACR11Unorm;
			case texture_format::EACRi11_Normalized: return wgpu::TextureFormat::EACR11Snorm;
			case texture_format::EACRGu11_Normalized: return wgpu::TextureFormat::EACRG11Unorm;
			case texture_format::EACRGi11_Normalized: return wgpu::TextureFormat::EACRG11Snorm;
			case texture_format::ASTC4x4_Normalized: return wgpu::TextureFormat::ASTC4x4Unorm;
			case texture_format::ASTC4x4_NormalizedSRGB: return wgpu::TextureFormat::ASTC4x4UnormSrgb;
			case texture_format::ASTC5x4_Normalized: return wgpu::TextureFormat::ASTC5x4Unorm;
			case texture_format::ASTC5x4_NormalizedSRGB: return wgpu::TextureFormat::ASTC5x4UnormSrgb;
			case texture_format::ASTC5x5_Normalized: return wgpu::TextureFormat::ASTC5x5Unorm;
			case texture_format::ASTC5x5_NormalizedSRGB: return wgpu::TextureFormat::ASTC5x5UnormSrgb;
			case texture_format::ASTC6x5_Normalized: return wgpu::TextureFormat::ASTC6x5Unorm;
			case texture_format::ASTC6x5_NormalizedSRGB: return wgpu::TextureFormat::ASTC6x5UnormSrgb;
			case texture_format::ASTC6x6_Normalized: return wgpu::TextureFormat::ASTC6x6Unorm;
			case texture_format::ASTC6x6_NormalizedSRGB: return wgpu::TextureFormat::ASTC6x6UnormSrgb;
			case texture_format::ASTC8x5_Normalized: return wgpu::TextureFormat::ASTC8x5Unorm;
			case texture_format::ASTC8x5_NormalizedSRGB: return wgpu::TextureFormat::ASTC8x5UnormSrgb;
			case texture_format::ASTC8x6_Normalized: return wgpu::TextureFormat::ASTC8x6Unorm;
			case texture_format::ASTC8x6_NormalizedSRGB: return wgpu::TextureFormat::ASTC8x6UnormSrgb;
			case texture_format::ASTC8x8_Normalized: return wgpu::TextureFormat::ASTC8x8Unorm;
			case texture_format::ASTC8x8_NormalizedSRGB: return wgpu::TextureFormat::ASTC8x8UnormSrgb;
			case texture_format::ASTC10x5_Normalized: return wgpu::TextureFormat::ASTC10x5Unorm;
			case texture_format::ASTC10x5_NormalizedSRGB: return wgpu::TextureFormat::ASTC10x5UnormSrgb;
			case texture_format::ASTC10x6_Normalized: return wgpu::TextureFormat::ASTC10x6Unorm;
			case texture_format::ASTC10x6_NormalizedSRGB: return wgpu::TextureFormat::ASTC10x6UnormSrgb;
			case texture_format::ASTC10x8_Normalized: return wgpu::TextureFormat::ASTC10x8Unorm;
			case texture_format::ASTC10x8_NormalizedSRGB: return wgpu::TextureFormat::ASTC10x8UnormSrgb;
			case texture_format::ASTC10x10_Normalized: return wgpu::TextureFormat::ASTC10x10Unorm;
			case texture_format::ASTC10x10_NormalizedSRGB: return wgpu::TextureFormat::ASTC10x10UnormSrgb;
			case texture_format::ASTC12x10_Normalized: return wgpu::TextureFormat::ASTC12x10Unorm;
			case texture_format::ASTC12x10_NormalizedSRGB: return wgpu::TextureFormat::ASTC12x10UnormSrgb;
			case texture_format::ASTC12x12_Normalized: return wgpu::TextureFormat::ASTC12x12Unorm;
			case texture_format::ASTC12x12_NormalizedSRGB: return wgpu::TextureFormat::ASTC12x12UnormSrgb;
			case texture_format::Ru16_Normalized: return wgpu::TextureFormat::R16Unorm;
			case texture_format::RGu16_Normalized: return wgpu::TextureFormat::RG16Unorm;
			case texture_format::RGBAu16_Normalized: return wgpu::TextureFormat::RGBA16Unorm;
			case texture_format::Ri16_Normalized: return wgpu::TextureFormat::R16Snorm;
			case texture_format::RGi16_Normalized: return wgpu::TextureFormat::RG16Snorm;
			case texture_format::RGBAi16_Normalized: return wgpu::TextureFormat::RGBA16Snorm;
			case texture_format::R8BG8Biplanar420_Normalized: return wgpu::TextureFormat::R8BG8Biplanar420Unorm;
			case texture_format::R10X6BG10X6Biplanar420_Normalized: return wgpu::TextureFormat::R10X6BG10X6Biplanar420Unorm;
			case texture_format::R8BG8A8Triplanar420_Normalized: return wgpu::TextureFormat::R8BG8A8Triplanar420Unorm;
			case texture_format::R8BG8Biplanar422_Normalized: return wgpu::TextureFormat::R8BG8Biplanar422Unorm;
			case texture_format::R8BG8Biplanar444_Normalized: return wgpu::TextureFormat::R8BG8Biplanar444Unorm;
			case texture_format::R10X6BG10X6Biplanar422_Normalized: return wgpu::TextureFormat::R10X6BG10X6Biplanar422Unorm;
			case texture_format::R10X6BG10X6Biplanar444_Normalized: return wgpu::TextureFormat::R10X6BG10X6Biplanar444Unorm;
		}
		STYLIZER_API_THROW("Unknown Texture Format: " + std::string(magic_enum::enum_name(format)));
	}

	inline texture_format from_wgpu(wgpu::TextureFormat format) {
		switch(format) {
			case wgpu::TextureFormat::Undefined: return texture_format::Undefined;
			case wgpu::TextureFormat::R8Unorm: return texture_format::Ru8_Normalized;
			case wgpu::TextureFormat::R8Snorm: return texture_format::Ri8_Normalized;
			case wgpu::TextureFormat::R8Uint: return texture_format::Ru8;
			case wgpu::TextureFormat::R8Sint: return texture_format::Ri8;
			case wgpu::TextureFormat::R16Uint: return texture_format::Ru16;
			case wgpu::TextureFormat::R16Sint: return texture_format::Ri16;
			case wgpu::TextureFormat::R16Float: return texture_format::Rf16;
			case wgpu::TextureFormat::RG8Unorm: return texture_format::RGu8_Normalized;
			case wgpu::TextureFormat::RG8Snorm: return texture_format::RGi8_Normalized;
			case wgpu::TextureFormat::RG8Uint: return texture_format::RGu8;
			case wgpu::TextureFormat::RG8Sint: return texture_format::RGi8;
			case wgpu::TextureFormat::R32Float: return texture_format::Rf32;
			case wgpu::TextureFormat::R32Uint: return texture_format::Ru32;
			case wgpu::TextureFormat::R32Sint: return texture_format::Ri32;
			case wgpu::TextureFormat::RG16Uint: return texture_format::RGu16;
			case wgpu::TextureFormat::RG16Sint: return texture_format::RGi16;
			case wgpu::TextureFormat::RG16Float: return texture_format::RGf16;
			case wgpu::TextureFormat::RGBA8Unorm: return texture_format::RGBAu8_Normalized;
			case wgpu::TextureFormat::RGBA8UnormSrgb: return texture_format::RGBAu8_NormalizedSRGB;
			case wgpu::TextureFormat::RGBA8Snorm: return texture_format::RGBAi8_Normalized;
			case wgpu::TextureFormat::RGBA8Uint: return texture_format::RGBAu8;
			case wgpu::TextureFormat::RGBA8Sint: return texture_format::RGBAi8;
			case wgpu::TextureFormat::BGRA8Unorm: return texture_format::BGRAu8_Normalized;
			case wgpu::TextureFormat::BGRA8UnormSrgb: return texture_format::BGRAu8_NormalizedSRGB;
			case wgpu::TextureFormat::RGB10A2Uint: return texture_format::RGBu10Au2;
			case wgpu::TextureFormat::RGB10A2Unorm: return texture_format::RGBu10Au2_Normalized;
			case wgpu::TextureFormat::RG11B10Ufloat: return texture_format::RGf11Bf10;
			case wgpu::TextureFormat::RG32Float: return texture_format::RGf32;
			case wgpu::TextureFormat::RG32Uint: return texture_format::RGu32;
			case wgpu::TextureFormat::RG32Sint: return texture_format::RGi32;
			case wgpu::TextureFormat::RGBA16Uint: return texture_format::RGBAu16;
			case wgpu::TextureFormat::RGBA16Sint: return texture_format::RGBAi16;
			case wgpu::TextureFormat::RGBA16Float: return texture_format::RGBAf16;
			case wgpu::TextureFormat::RGBA32Float: return texture_format::RGBAf32;
			case wgpu::TextureFormat::RGBA32Uint: return texture_format::RGBAu32;
			case wgpu::TextureFormat::RGBA32Sint: return texture_format::RGBAi32;
			case wgpu::TextureFormat::Stencil8: return texture_format::Stencil_u8;
			case wgpu::TextureFormat::Depth16Unorm: return texture_format::Depth_u16;
			case wgpu::TextureFormat::Depth24Plus: return texture_format::Depth_u24;
			case wgpu::TextureFormat::Depth24PlusStencil8: return texture_format::Depth_u24Stencil_u8;
			case wgpu::TextureFormat::Depth32Float: return texture_format::Depth_f32;
			case wgpu::TextureFormat::Depth32FloatStencil8: return texture_format::Depth_f32Stencil_u8;
			case wgpu::TextureFormat::BC1RGBAUnorm: return texture_format::BC1RGBA_Normalized;
			case wgpu::TextureFormat::BC1RGBAUnormSrgb: return texture_format::BC1RGBA_NormalizedSRGB;
			case wgpu::TextureFormat::BC2RGBAUnorm: return texture_format::BC2RGBA_Normalized;
			case wgpu::TextureFormat::BC2RGBAUnormSrgb: return texture_format::BC2RGBA_NormalizedSRGB;
			case wgpu::TextureFormat::BC3RGBAUnorm: return texture_format::BC3RGBA_Normalized;
			case wgpu::TextureFormat::BC3RGBAUnormSrgb: return texture_format::BC3RGBA_NormalizedSRGB;
			case wgpu::TextureFormat::BC7RGBAUnorm: return texture_format::BC7RGBA_Normalized;
			case wgpu::TextureFormat::BC7RGBAUnormSrgb: return texture_format::BC7RGBA_NormalizedSRGB;
			case wgpu::TextureFormat::ETC2RGB8Unorm: return texture_format::ETC2RGB8_Normalized;
			case wgpu::TextureFormat::ETC2RGB8UnormSrgb: return texture_format::ETC2RGB8_NormalizedSRGB;
			case wgpu::TextureFormat::ETC2RGB8A1Unorm: return texture_format::ETC2RGB8A1_Normalized;
			case wgpu::TextureFormat::ETC2RGB8A1UnormSrgb: return texture_format::ETC2RGB8A1_NormalizedSRGB;
			case wgpu::TextureFormat::ETC2RGBA8Unorm: return texture_format::ETC2RGBA8_Normalized;
			case wgpu::TextureFormat::ETC2RGBA8UnormSrgb: return texture_format::ETC2RGBA8_NormalizedSRGB;
			case wgpu::TextureFormat::EACR11Unorm: return texture_format::EACRu11_Normalized;
			case wgpu::TextureFormat::EACR11Snorm: return texture_format::EACRi11_Normalized;
			case wgpu::TextureFormat::EACRG11Unorm: return texture_format::EACRGu11_Normalized;
			case wgpu::TextureFormat::EACRG11Snorm: return texture_format::EACRGi11_Normalized;
			case wgpu::TextureFormat::ASTC4x4Unorm: return texture_format::ASTC4x4_Normalized;
			case wgpu::TextureFormat::ASTC4x4UnormSrgb: return texture_format::ASTC4x4_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC5x4Unorm: return texture_format::ASTC5x4_Normalized;
			case wgpu::TextureFormat::ASTC5x4UnormSrgb: return texture_format::ASTC5x4_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC5x5Unorm: return texture_format::ASTC5x5_Normalized;
			case wgpu::TextureFormat::ASTC5x5UnormSrgb: return texture_format::ASTC5x5_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC6x5Unorm: return texture_format::ASTC6x5_Normalized;
			case wgpu::TextureFormat::ASTC6x5UnormSrgb: return texture_format::ASTC6x5_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC6x6Unorm: return texture_format::ASTC6x6_Normalized;
			case wgpu::TextureFormat::ASTC6x6UnormSrgb: return texture_format::ASTC6x6_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC8x5Unorm: return texture_format::ASTC8x5_Normalized;
			case wgpu::TextureFormat::ASTC8x5UnormSrgb: return texture_format::ASTC8x5_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC8x6Unorm: return texture_format::ASTC8x6_Normalized;
			case wgpu::TextureFormat::ASTC8x6UnormSrgb: return texture_format::ASTC8x6_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC8x8Unorm: return texture_format::ASTC8x8_Normalized;
			case wgpu::TextureFormat::ASTC8x8UnormSrgb: return texture_format::ASTC8x8_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC10x5Unorm: return texture_format::ASTC10x5_Normalized;
			case wgpu::TextureFormat::ASTC10x5UnormSrgb: return texture_format::ASTC10x5_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC10x6Unorm: return texture_format::ASTC10x6_Normalized;
			case wgpu::TextureFormat::ASTC10x6UnormSrgb: return texture_format::ASTC10x6_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC10x8Unorm: return texture_format::ASTC10x8_Normalized;
			case wgpu::TextureFormat::ASTC10x8UnormSrgb: return texture_format::ASTC10x8_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC10x10Unorm: return texture_format::ASTC10x10_Normalized;
			case wgpu::TextureFormat::ASTC10x10UnormSrgb: return texture_format::ASTC10x10_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC12x10Unorm: return texture_format::ASTC12x10_Normalized;
			case wgpu::TextureFormat::ASTC12x10UnormSrgb: return texture_format::ASTC12x10_NormalizedSRGB;
			case wgpu::TextureFormat::ASTC12x12Unorm: return texture_format::ASTC12x12_Normalized;
			case wgpu::TextureFormat::ASTC12x12UnormSrgb: return texture_format::ASTC12x12_NormalizedSRGB;
			case wgpu::TextureFormat::R16Unorm: return texture_format::Ru16_Normalized;
			case wgpu::TextureFormat::RG16Unorm: return texture_format::RGu16_Normalized;
			case wgpu::TextureFormat::RGBA16Unorm: return texture_format::RGBAu16_Normalized;
			case wgpu::TextureFormat::R16Snorm: return texture_format::Ri16_Normalized;
			case wgpu::TextureFormat::RG16Snorm: return texture_format::RGi16_Normalized;
			case wgpu::TextureFormat::RGBA16Snorm: return texture_format::RGBAi16_Normalized;
			case wgpu::TextureFormat::R8BG8Biplanar420Unorm: return texture_format::R8BG8Biplanar420_Normalized;
			case wgpu::TextureFormat::R10X6BG10X6Biplanar420Unorm: return texture_format::R10X6BG10X6Biplanar420_Normalized;
			case wgpu::TextureFormat::R8BG8A8Triplanar420Unorm: return texture_format::R8BG8A8Triplanar420_Normalized;
			case wgpu::TextureFormat::R8BG8Biplanar422Unorm: return texture_format::R8BG8Biplanar422_Normalized;
			case wgpu::TextureFormat::R8BG8Biplanar444Unorm: return texture_format::R8BG8Biplanar444_Normalized;
			case wgpu::TextureFormat::R10X6BG10X6Biplanar422Unorm: return texture_format::R10X6BG10X6Biplanar422_Normalized;
			case wgpu::TextureFormat::R10X6BG10X6Biplanar444Unorm: return texture_format::R10X6BG10X6Biplanar444_Normalized;
			default: STYLIZER_API_THROW("Unknown Texture Format");
		}
	}

	inline wgpu::TextureUsage to_wgpu_texture(usage usage) {
		wgpu::Flags out = {};
		if((usage & usage::CopySource) > usage::Invalid) {
			out |= wgpu::TextureUsage::CopySrc;
			usage &= ~usage::CopySource;
		}
		if((usage & usage::CopyDestination) > usage::Invalid) {
			out |= wgpu::TextureUsage::CopyDst;
			usage &= ~usage::CopyDestination;
		}
		if((usage & usage::Storage) > usage::Invalid) {
			out |= wgpu::TextureUsage::StorageBinding;
			usage &= ~usage::Storage;
		}
		if((usage & usage::RenderAttachment) > usage::Invalid) {
			out |= wgpu::TextureUsage::RenderAttachment;
			usage &= ~usage::RenderAttachment;
		}
		if((usage & usage::TextureBinding) > usage::Invalid) {
			out |= wgpu::TextureUsage::TextureBinding;
			usage &= ~usage::TextureBinding;
		}

		if(usage > usage::Invalid)
			STYLIZER_API_THROW("Invalid Texture Usage(s): " + std::string(magic_enum::enum_flags_name(usage)));
		return out;
	}
	inline usage from_wgpu(wgpu::TextureUsage usage) {
		auto out = usage::Invalid;
		if(usage & wgpu::TextureUsage::CopySrc)
			out |= usage::CopySource;
		if(usage & wgpu::TextureUsage::CopyDst)
			out |= usage::CopyDestination;
		if(usage & wgpu::TextureUsage::RenderAttachment)
			out |= usage::RenderAttachment;
		if(usage & wgpu::TextureUsage::TextureBinding)
			out |= usage::TextureBinding;
		// TODO: Need some form of validation?
		return out;
	}

	inline wgpu::BufferUsage to_wgpu_buffer(usage usage) {
		wgpu::Flags out = {};
		if((usage & usage::CopySource) > usage::Invalid) {
			out |= wgpu::BufferUsage::CopySrc;
			usage &= ~usage::CopySource;
		}
		if((usage & usage::CopyDestination) > usage::Invalid) {
			out |= wgpu::BufferUsage::CopyDst;
			usage &= ~usage::CopyDestination;
		}
		if((usage & usage::Vertex) > usage::Invalid) {
			out |= wgpu::BufferUsage::Vertex;
			usage &= ~usage::Vertex;
		}
		if((usage & usage::Index) > usage::Invalid) {
			out |= wgpu::BufferUsage::Index;
			usage &= ~usage::Index;
		}
		if((usage & usage::Storage) > usage::Invalid) {
			out |= wgpu::BufferUsage::Storage;
			usage &= ~usage::Storage;
		}
		if((usage & usage::Uniform) > usage::Invalid) {
			out |= wgpu::BufferUsage::Uniform;
			usage &= ~usage::Uniform;
		}
		if((usage & usage::MapRead) > usage::Invalid) {
			out |= wgpu::BufferUsage::MapRead;
			usage &= ~usage::MapRead;
		}
		if((usage & usage::MapWrite) > usage::Invalid) {
			out |= wgpu::BufferUsage::MapWrite;
			usage &= ~usage::MapWrite;
		}
		if(usage > usage::Invalid)
			STYLIZER_API_THROW("Invalid Buffer Usage(s): " + std::string(magic_enum::enum_flags_name(usage)));
		return out;
	}

	inline wgpu::CompareFunction to_wgpu(comparison_function func) {
		switch(func){
			case comparison_function::NoDepthCompare: return wgpu::CompareFunction::Undefined;
			case comparison_function::Never: return wgpu::CompareFunction::Never;
			case comparison_function::Less: return wgpu::CompareFunction::Less;
			case comparison_function::LessEqual: return wgpu::CompareFunction::LessEqual;
			case comparison_function::Greater: return wgpu::CompareFunction::Greater;
			case comparison_function::GreaterEqual: return wgpu::CompareFunction::GreaterEqual;
			case comparison_function::Equal: return wgpu::CompareFunction::Equal;
			case comparison_function::NotEqual: return wgpu::CompareFunction::NotEqual;
			case comparison_function::Always: return wgpu::CompareFunction::Always;
		}
		STYLIZER_API_THROW("Unknown Comparison Function: " + std::string(magic_enum::enum_name(func)));
	}

	inline wgpu::PresentMode to_wgpu(surface::present_mode mode) {
		switch(mode){
			case surface::present_mode::Fifo: return wgpu::PresentMode::Fifo;
			case surface::present_mode::FifoRelaxed: return wgpu::PresentMode::FifoRelaxed;
			case surface::present_mode::Immediate: return wgpu::PresentMode::Immediate;
			case surface::present_mode::Mailbox: return wgpu::PresentMode::Mailbox;
		}
		STYLIZER_API_THROW("Unknown Present Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::AddressMode to_wgpu(texture::address_mode mode) {
		switch(mode){
			case texture::address_mode::Repeat: return wgpu::AddressMode::Repeat;
			case texture::address_mode::MirrorRepeat: return wgpu::AddressMode::MirrorRepeat;
			case texture::address_mode::ClampToEdge: return wgpu::AddressMode::ClampToEdge;
		}
		STYLIZER_API_THROW("Unknown Address Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::VertexFormat to_wgpu(enum render_pipeline::config::vertex_buffer_layout::attribute::format format) {
		using fmt = enum render_pipeline::config::vertex_buffer_layout::attribute::format;
		switch(format){
			case fmt::f32x1: return wgpu::VertexFormat::Float32;
			case fmt::f32x2: return wgpu::VertexFormat::Float32x2;
			case fmt::f32x3: return wgpu::VertexFormat::Float32x3;
			case fmt::f32x4: return wgpu::VertexFormat::Float32x4;
			case fmt::i32x1: return wgpu::VertexFormat::Sint32;
			case fmt::i32x2: return wgpu::VertexFormat::Sint32x2;
			case fmt::i32x3: return wgpu::VertexFormat::Sint32x3;
			case fmt::i32x4: return wgpu::VertexFormat::Sint32x4;
			case fmt::u32x1: return wgpu::VertexFormat::Uint32;
			case fmt::u32x2: return wgpu::VertexFormat::Uint32x2;
			case fmt::u32x3: return wgpu::VertexFormat::Uint32x3;
			case fmt::u32x4: return wgpu::VertexFormat::Uint32x4;
		}
		STYLIZER_API_THROW("Unknown Vertex Format: " + std::string(magic_enum::enum_name(format)));
	}

	inline wgpu::PrimitiveTopology to_wgpu(enum render_pipeline::config::primitive_topology topology) {
		switch(topology){
			case render_pipeline::config::primitive_topology::PointList: return wgpu::PrimitiveTopology::PointList;
			case render_pipeline::config::primitive_topology::LineList: return wgpu::PrimitiveTopology::LineList;
			case render_pipeline::config::primitive_topology::LineStrip: return wgpu::PrimitiveTopology::LineStrip;
			case render_pipeline::config::primitive_topology::TriangleList: return wgpu::PrimitiveTopology::TriangleList;
			case render_pipeline::config::primitive_topology::TriangleStrip: return wgpu::PrimitiveTopology::TriangleStrip;
		}
		STYLIZER_API_THROW("Unknown Primitive Topology: " + std::string(magic_enum::enum_name(topology)));
	}

	inline wgpu::CullMode to_wgpu(enum render_pipeline::config::cull_mode mode) {
		switch(mode){
			case render_pipeline::config::cull_mode::Back: return wgpu::CullMode::Back;
			case render_pipeline::config::cull_mode::Front: return wgpu::CullMode::Front;
			// case render_pipeline::config::cull_mode::Both: throw
			case render_pipeline::config::cull_mode::Neither: return wgpu::CullMode::None;
		}
		STYLIZER_API_THROW("Unknown Cull Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::BlendOperation to_wgpu(enum blend_state::operation op) {
		switch(op){
			case blend_state::operation::Add: return wgpu::BlendOperation::Add;
			case blend_state::operation::Subtract: return wgpu::BlendOperation::Subtract;
			case blend_state::operation::ReverseSubtract: return wgpu::BlendOperation::ReverseSubtract;
			case blend_state::operation::Min: return wgpu::BlendOperation::Min;
			case blend_state::operation::Max: return wgpu::BlendOperation::Max;
		}
		STYLIZER_API_THROW("Unknown Blend Operation: " + std::string(magic_enum::enum_name(op)));
	}

	inline wgpu::BlendFactor to_wgpu(enum blend_state::factor factor) {
		switch(factor){
			case blend_state::factor::Zero: return wgpu::BlendFactor::Zero;
			case blend_state::factor::One: return wgpu::BlendFactor::One;
			case blend_state::factor::Src: return wgpu::BlendFactor::Src;
			case blend_state::factor::OneMinusSrc: return wgpu::BlendFactor::OneMinusSrc;
			case blend_state::factor::SrcAlpha: return wgpu::BlendFactor::SrcAlpha;
			case blend_state::factor::OneMinusSrcAlpha: return wgpu::BlendFactor::OneMinusSrcAlpha;
			case blend_state::factor::Dst: return wgpu::BlendFactor::Dst;
			case blend_state::factor::OneMinusDst: return wgpu::BlendFactor::OneMinusDst;
			case blend_state::factor::DstAlpha: return wgpu::BlendFactor::DstAlpha;
			case blend_state::factor::OneMinusDstAlpha: return wgpu::BlendFactor::OneMinusDstAlpha;
			case blend_state::factor::SrcAlphaSaturated: return wgpu::BlendFactor::SrcAlphaSaturated;
			case blend_state::factor::Constant: return wgpu::BlendFactor::Constant;
			case blend_state::factor::OneMinusConstant: return wgpu::BlendFactor::OneMinusConstant;
		}
		STYLIZER_API_THROW("Unknown Blend Factor: " + std::string(magic_enum::enum_name(factor)));
	}
}