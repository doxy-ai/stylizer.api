#pragma once

#include "webgpu.hpp"
#include "cstring_from_view.hpp"

#include <chrono>
#include <string_view>
#include <utility>

namespace stylizer::api::webgpu {
	// Implementation in device.cpp
	WGPUInstance get_common_instance();

	inline WGPUStringView to_webgpu(std::string_view view) { return {view.data(), view.size()}; }
	inline std::string_view from_webgpu(WGPUStringView view) { return {view.data, view.length}; }

	inline WGPUCommandEncoder create_command_encoder(WGPUDevice device, std::string_view label = "Temporary Encode") {
		WGPUCommandEncoderDescriptor d = WGPU_COMMAND_ENCODER_DESCRIPTOR_INIT;
		d.label = to_webgpu(label);
		return wgpuDeviceCreateCommandEncoder(device, &d);
	}

	inline void finish_and_submit(WGPUCommandEncoder e, WGPUQueue q) {
		WGPUCommandBufferDescriptor d = WGPU_COMMAND_BUFFER_DESCRIPTOR_INIT;
		d.label = to_webgpu("Temporary");
		auto commands = wgpuCommandEncoderFinish(e, &d);
		defer_ { wgpuCommandBufferRelease(commands); };
		wgpuQueueSubmit(q, 1, &commands);
	}

	inline bool wait_for_future(WGPUFuture future, std::chrono::nanoseconds timeout = std::chrono::milliseconds(1)) {
		WGPUFutureWaitInfo info = WGPU_FUTURE_WAIT_INFO_INIT;
		info.future = future;
		auto status = wgpuInstanceWaitAny(get_common_instance(), 1, &info, timeout.count());
		return status == WGPUWaitStatus_Success;
	}
	inline bool wait_for_any_futures(std::span<WGPUFuture> futures, std::chrono::nanoseconds timeout = std::chrono::milliseconds(1)) {
		std::vector<WGPUFutureWaitInfo> infos(futures.size(), WGPU_FUTURE_WAIT_INFO_INIT);
		for (size_t i = 0; i < futures.size(); ++i)
			infos[i].future = futures[i];
		auto status = wgpuInstanceWaitAny(get_common_instance(), infos.size(), infos.data(), timeout.count());
		return status == WGPUWaitStatus_Success;
	}
	inline size_t wait_for_all_futures(std::span<WGPUFuture> futures, std::chrono::nanoseconds timeout = std::chrono::milliseconds(1)) {
		for (size_t i = 0; i < futures.size(); ++i)
			if (!wait_for_future(futures[i], timeout / futures.size()))
				return i;
		return -1;
	}

	template<typename Tto, typename Tfrom>
	inline Tto& confirm_webgpu_type(Tfrom& ref) {
		auto& res = ref.template as<Tto>();
		assert(res.type == magic_number);
		return res;
	}
	template<typename Tto, typename Tfrom>
	inline const Tto& confirm_webgpu_type(const Tfrom& ref) {
		auto& res = const_cast<Tfrom&>(ref).template as<Tto>();
		assert(res.type == magic_number);
		return res;
	}

	inline WGPUColor to_webgpu(color32 c) { return WGPUColor{c.r, c.g, c.b, c.a}; }
	inline color32 from_webgpu(WGPUColor c) { return color32{(stdmath::stl::float32_t)c.r, (stdmath::stl::float32_t)c.g, (stdmath::stl::float32_t)c.b, (stdmath::stl::float32_t)c.a}; }

	inline WGPUOptionalBool to_webgpu(bool v) { return v ? WGPUOptionalBool_True : WGPUOptionalBool_False; }

	inline usage from_webgpu_texture(WGPUTextureUsage usage) {
		enum usage out = usage::Invalid;
		if(flags_set(usage, WGPUTextureUsage_CopySrc)) out |= usage::CopySource;
		if(flags_set(usage, WGPUTextureUsage_CopyDst)) out |= usage::CopyDestination;
		if(flags_set(usage, WGPUTextureUsage_TextureBinding)) out |= usage::Texture;
		if(flags_set(usage, WGPUTextureUsage_StorageBinding)) out |= usage::Storage;
		if(flags_set(usage, WGPUTextureUsage_RenderAttachment)) out |= usage::RenderAttachment;
		if(flags_set(usage, WGPUTextureUsage_TransientAttachment)) out |= usage::TransientAttachment;
		if(flags_set(usage, WGPUTextureUsage_StorageAttachment)) out |= usage::StorageAttachment;
		if(out == usage::Invalid) STYLIZER_API_THROW(std::string("Failed to find texture usage: ") + std::to_string(usage));
		return out;
	}

	inline WGPUTextureUsage to_webgpu_texture(usage usage) {
		WGPUTextureUsage out = 0;
		if(flags_set(usage, usage::CopySource)) out |= WGPUTextureUsage_CopySrc;
		if(flags_set(usage, usage::CopyDestination)) out |= WGPUTextureUsage_CopyDst;
		if(flags_set(usage, usage::Texture)) out |= WGPUTextureUsage_TextureBinding;
		if(flags_set(usage, usage::Storage)) out |= WGPUTextureUsage_StorageBinding;
		if(flags_set(usage, usage::RenderAttachment)) out |= WGPUTextureUsage_RenderAttachment;
		if(flags_set(usage, usage::TransientAttachment)) out |= WGPUTextureUsage_TransientAttachment;
		if(flags_set(usage, usage::StorageAttachment)) out |= WGPUTextureUsage_StorageAttachment;
		if(out == 0) STYLIZER_API_THROW(std::string("Failed to find texture usage: ") + std::string(magic_enum::enum_name(usage)));
		return out;
	}

	inline comparison_function from_webgpu(WGPUCompareFunction func) {
		switch (func) {
		case WGPUCompareFunction_Undefined: return comparison_function::NoDepthCompare;
		case WGPUCompareFunction_Never: return comparison_function::Never;
		case WGPUCompareFunction_Less: return comparison_function::Less;
		case WGPUCompareFunction_LessEqual: return comparison_function::LessEqual;
		case WGPUCompareFunction_Greater: return comparison_function::Greater;
		case WGPUCompareFunction_GreaterEqual: return comparison_function::GreaterEqual;
		case WGPUCompareFunction_Equal: return comparison_function::Equal;
		case WGPUCompareFunction_NotEqual: return comparison_function::NotEqual;
		case WGPUCompareFunction_Always: return comparison_function::Always;
		default:
			STYLIZER_API_THROW(std::string("Failed to find compare function: ") + std::string(magic_enum::enum_name(func)));
		}
		std::unreachable();
	}

	inline WGPUCompareFunction to_webgpu(comparison_function func) {
		switch (func) {
		case comparison_function::NoDepthCompare: return WGPUCompareFunction_Undefined;
		case comparison_function::Never: return WGPUCompareFunction_Never;
		case comparison_function::Less: return WGPUCompareFunction_Less;
		case comparison_function::LessEqual: return WGPUCompareFunction_LessEqual;
		case comparison_function::Greater: return WGPUCompareFunction_Greater;
		case comparison_function::GreaterEqual: return WGPUCompareFunction_GreaterEqual;
		case comparison_function::Equal: return WGPUCompareFunction_Equal;
		case comparison_function::NotEqual: return WGPUCompareFunction_NotEqual;
		case comparison_function::Always: return WGPUCompareFunction_Always;
		}
		std::unreachable();
	}


	inline bool is_texture_format_srgb(WGPUTextureFormat format) {
		switch(format){
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
		}
	}

	inline texture_format from_webgpu(WGPUTextureFormat format) {
		switch(format) {
		case WGPUTextureFormat_Undefined: return texture_format::Undefined;
		case WGPUTextureFormat_R8Unorm: return texture_format::Ru8_Normalized;
		case WGPUTextureFormat_R8Snorm: return texture_format::Ri8_Normalized;
		case WGPUTextureFormat_R8Uint: return texture_format::Ru8;
		case WGPUTextureFormat_R8Sint: return texture_format::Ri8;
		case WGPUTextureFormat_R16Uint: return texture_format::Ru16;
		case WGPUTextureFormat_R16Sint: return texture_format::Ri16;
		case WGPUTextureFormat_R16Float: return texture_format::Rf16;
		case WGPUTextureFormat_RG8Unorm: return texture_format::RGu8_Normalized;
		case WGPUTextureFormat_RG8Snorm: return texture_format::RGi8_Normalized;
		case WGPUTextureFormat_RG8Uint: return texture_format::RGu8;
		case WGPUTextureFormat_RG8Sint: return texture_format::RGi8;
		case WGPUTextureFormat_R32Float: return texture_format::Rf32;
		case WGPUTextureFormat_R32Uint: return texture_format::Ru32;
		case WGPUTextureFormat_R32Sint: return texture_format::Ri32;
		case WGPUTextureFormat_RG16Uint: return texture_format::RGu16;
		case WGPUTextureFormat_RG16Sint: return texture_format::RGi16;
		case WGPUTextureFormat_RG16Float: return texture_format::RGf16;
		case WGPUTextureFormat_RGBA8Unorm: return texture_format::RGBAu8_Normalized;
		case WGPUTextureFormat_RGBA8UnormSrgb: return texture_format::RGBAu8_NormalizedSRGB;
		case WGPUTextureFormat_RGBA8Snorm: return texture_format::RGBAi8_Normalized;
		case WGPUTextureFormat_RGBA8Uint: return texture_format::RGBAu8;
		case WGPUTextureFormat_RGBA8Sint: return texture_format::RGBAi8;
		case WGPUTextureFormat_BGRA8Unorm: return texture_format::BGRAu8_Normalized;
		case WGPUTextureFormat_BGRA8UnormSrgb: return texture_format::BGRAu8_NormalizedSRGB;
		case WGPUTextureFormat_RGB10A2Uint: return texture_format::RGBu10Au2;
		case WGPUTextureFormat_RGB10A2Unorm: return texture_format::RGBu10Au2_Normalized;
		case WGPUTextureFormat_RG11B10Ufloat: return texture_format::RGf11Bf10;
		// case WGPUTextureFormat_RGB9E5Ufloat: return texture_format::RGB;
		case WGPUTextureFormat_RG32Float: return texture_format::RGf32;
		case WGPUTextureFormat_RG32Uint: return texture_format::RGu32;
		case WGPUTextureFormat_RG32Sint: return texture_format::RGi32;
		case WGPUTextureFormat_RGBA16Uint: return texture_format::RGBAu16;
		case WGPUTextureFormat_RGBA16Sint: return texture_format::RGBAi16;
		case WGPUTextureFormat_RGBA16Float: return texture_format::RGBAf16;
		case WGPUTextureFormat_RGBA32Float: return texture_format::RGBAf32;
		case WGPUTextureFormat_RGBA32Uint: return texture_format::RGBAu32;
		case WGPUTextureFormat_RGBA32Sint: return texture_format::RGBAi32;
		case WGPUTextureFormat_Stencil8: return texture_format::Stencil_u8;
		case WGPUTextureFormat_Depth16Unorm: return texture_format::Depth_u16;
		case WGPUTextureFormat_Depth24Plus: return texture_format::Depth_u24;
		case WGPUTextureFormat_Depth24PlusStencil8: return texture_format::Depth_u24Stencil_u8;
		case WGPUTextureFormat_Depth32Float: return texture_format::Depth_f32;
		case WGPUTextureFormat_Depth32FloatStencil8: return texture_format::Depth_f32Stencil_u8;
		case WGPUTextureFormat_BC1RGBAUnorm: return texture_format::BC1RGBA_Normalized;
		case WGPUTextureFormat_BC1RGBAUnormSrgb: return texture_format::BC1RGBA_NormalizedSRGB;
		case WGPUTextureFormat_BC2RGBAUnorm: return texture_format::BC2RGBA_Normalized;
		case WGPUTextureFormat_BC2RGBAUnormSrgb: return texture_format::BC2RGBA_NormalizedSRGB;
		case WGPUTextureFormat_BC3RGBAUnorm: return texture_format::BC3RGBA_Normalized;
		case WGPUTextureFormat_BC3RGBAUnormSrgb: return texture_format::BC3RGBA_NormalizedSRGB;
		case WGPUTextureFormat_BC4RUnorm: return texture_format::BC4Ru_Normalized;
		case WGPUTextureFormat_BC4RSnorm: return texture_format::BC4Ri_Normalized;
		case WGPUTextureFormat_BC5RGUnorm: return texture_format::BC5RGu_Normalized;
		case WGPUTextureFormat_BC5RGSnorm: return texture_format::BC5RGi_Normalized;
		case WGPUTextureFormat_BC6HRGBUfloat: return texture_format::BC6HRGBUfloat;
		case WGPUTextureFormat_BC6HRGBFloat: return texture_format::BC6HRGBFloat;
		case WGPUTextureFormat_BC7RGBAUnorm: return texture_format::BC7RGBA_Normalized;
		case WGPUTextureFormat_BC7RGBAUnormSrgb: return texture_format::BC7RGBA_NormalizedSRGB;
		case WGPUTextureFormat_ETC2RGB8Unorm: return texture_format::ETC2RGB8_Normalized;
		case WGPUTextureFormat_ETC2RGB8UnormSrgb: return texture_format::ETC2RGB8_NormalizedSRGB;
		case WGPUTextureFormat_ETC2RGB8A1Unorm: return texture_format::ETC2RGB8A1_Normalized;
		case WGPUTextureFormat_ETC2RGB8A1UnormSrgb: return texture_format::ETC2RGB8A1_NormalizedSRGB;
		case WGPUTextureFormat_ETC2RGBA8Unorm: return texture_format::ETC2RGBA8_Normalized;
		case WGPUTextureFormat_ETC2RGBA8UnormSrgb: return texture_format::ETC2RGBA8_NormalizedSRGB;
		case WGPUTextureFormat_EACR11Unorm: return texture_format::EACRu11_Normalized;
		case WGPUTextureFormat_EACR11Snorm: return texture_format::EACRi11_Normalized;
		case WGPUTextureFormat_EACRG11Unorm: return texture_format::EACRGu11_Normalized;
		case WGPUTextureFormat_EACRG11Snorm: return texture_format::EACRGi11_Normalized;
		case WGPUTextureFormat_ASTC4x4Unorm: return texture_format::ASTC4x4_Normalized;
		case WGPUTextureFormat_ASTC4x4UnormSrgb: return texture_format::ASTC4x4_NormalizedSRGB;
		case WGPUTextureFormat_ASTC5x4Unorm: return texture_format::ASTC5x4_Normalized;
		case WGPUTextureFormat_ASTC5x4UnormSrgb: return texture_format::ASTC5x4_NormalizedSRGB;
		case WGPUTextureFormat_ASTC5x5Unorm: return texture_format::ASTC5x5_Normalized;
		case WGPUTextureFormat_ASTC5x5UnormSrgb: return texture_format::ASTC5x5_NormalizedSRGB;
		case WGPUTextureFormat_ASTC6x5Unorm: return texture_format::ASTC6x5_Normalized;
		case WGPUTextureFormat_ASTC6x5UnormSrgb: return texture_format::ASTC6x5_NormalizedSRGB;
		case WGPUTextureFormat_ASTC6x6Unorm: return texture_format::ASTC6x6_Normalized;
		case WGPUTextureFormat_ASTC6x6UnormSrgb: return texture_format::ASTC6x6_NormalizedSRGB;
		case WGPUTextureFormat_ASTC8x5Unorm: return texture_format::ASTC8x5_Normalized;
		case WGPUTextureFormat_ASTC8x5UnormSrgb: return texture_format::ASTC8x5_NormalizedSRGB;
		case WGPUTextureFormat_ASTC8x6Unorm: return texture_format::ASTC8x6_Normalized;
		case WGPUTextureFormat_ASTC8x6UnormSrgb: return texture_format::ASTC8x6_NormalizedSRGB;
		case WGPUTextureFormat_ASTC8x8Unorm: return texture_format::ASTC8x8_Normalized;
		case WGPUTextureFormat_ASTC8x8UnormSrgb: return texture_format::ASTC8x8_NormalizedSRGB;
		case WGPUTextureFormat_ASTC10x5Unorm: return texture_format::ASTC10x5_Normalized;
		case WGPUTextureFormat_ASTC10x5UnormSrgb: return texture_format::ASTC10x5_NormalizedSRGB;
		case WGPUTextureFormat_ASTC10x6Unorm: return texture_format::ASTC10x6_Normalized;
		case WGPUTextureFormat_ASTC10x6UnormSrgb: return texture_format::ASTC10x6_NormalizedSRGB;
		case WGPUTextureFormat_ASTC10x8Unorm: return texture_format::ASTC10x8_Normalized;
		case WGPUTextureFormat_ASTC10x8UnormSrgb: return texture_format::ASTC10x8_NormalizedSRGB;
		case WGPUTextureFormat_ASTC10x10Unorm: return texture_format::ASTC10x10_Normalized;
		case WGPUTextureFormat_ASTC10x10UnormSrgb: return texture_format::ASTC10x10_NormalizedSRGB;
		case WGPUTextureFormat_ASTC12x10Unorm: return texture_format::ASTC12x10_Normalized;
		case WGPUTextureFormat_ASTC12x10UnormSrgb: return texture_format::ASTC12x10_NormalizedSRGB;
		case WGPUTextureFormat_ASTC12x12Unorm: return texture_format::ASTC12x12_Normalized;
		case WGPUTextureFormat_ASTC12x12UnormSrgb: return texture_format::ASTC12x12_NormalizedSRGB;
		case WGPUTextureFormat_R16Unorm: return texture_format::Ru16_Normalized;
		case WGPUTextureFormat_RG16Unorm: return texture_format::RGu16_Normalized;
		case WGPUTextureFormat_RGBA16Unorm: return texture_format::RGBAu16_Normalized;
		case WGPUTextureFormat_R16Snorm: return texture_format::Ri16_Normalized;
		case WGPUTextureFormat_RG16Snorm: return texture_format::RGi16_Normalized;
		case WGPUTextureFormat_RGBA16Snorm: return texture_format::RGBAi16_Normalized;
		case WGPUTextureFormat_R8BG8Biplanar420Unorm: return texture_format::R8BG8Biplanar420_Normalized;
		case WGPUTextureFormat_R10X6BG10X6Biplanar420Unorm: return texture_format::R10X6BG10X6Biplanar420_Normalized;
		case WGPUTextureFormat_R8BG8A8Triplanar420Unorm: return texture_format::R8BG8A8Triplanar420_Normalized;
		case WGPUTextureFormat_R8BG8Biplanar422Unorm: return texture_format::R8BG8Biplanar422_Normalized;
		case WGPUTextureFormat_R8BG8Biplanar444Unorm: return texture_format::R8BG8Biplanar444_Normalized;
		case WGPUTextureFormat_R10X6BG10X6Biplanar422Unorm: return texture_format::R10X6BG10X6Biplanar422_Normalized;
		case WGPUTextureFormat_R10X6BG10X6Biplanar444Unorm: return texture_format::R10X6BG10X6Biplanar444_Normalized;
		default:
			STYLIZER_API_THROW(std::string("Failed to find texture format: ") + std::string(magic_enum::enum_name(format)));
		}
	}

	inline WGPUTextureFormat to_webgpu(texture_format format) {
		switch(format) {
		case texture_format::Undefined: return WGPUTextureFormat_Undefined;
		case texture_format::Ru8_Normalized: return WGPUTextureFormat_R8Unorm;
		case texture_format::Ri8_Normalized: return WGPUTextureFormat_R8Snorm;
		case texture_format::Ru8: return WGPUTextureFormat_R8Uint;
		case texture_format::Ri8: return WGPUTextureFormat_R8Sint;
		case texture_format::Ru16: return WGPUTextureFormat_R16Uint;
		case texture_format::Ri16: return WGPUTextureFormat_R16Sint;
		case texture_format::Rf16: return WGPUTextureFormat_R16Float;
		case texture_format::RGu8_Normalized: return WGPUTextureFormat_RG8Unorm;
		case texture_format::RGi8_Normalized: return WGPUTextureFormat_RG8Snorm;
		case texture_format::RGu8: return WGPUTextureFormat_RG8Uint;
		case texture_format::RGi8: return WGPUTextureFormat_RG8Sint;
		case texture_format::Rf32: return WGPUTextureFormat_R32Float;
		case texture_format::Ru32: return WGPUTextureFormat_R32Uint;
		case texture_format::Ri32: return WGPUTextureFormat_R32Sint;
		case texture_format::RGu16: return WGPUTextureFormat_RG16Uint;
		case texture_format::RGi16: return WGPUTextureFormat_RG16Sint;
		case texture_format::RGf16: return WGPUTextureFormat_RG16Float;
		case texture_format::RGBAu8_Normalized: return WGPUTextureFormat_RGBA8Unorm;
		case texture_format::RGBAu8_NormalizedSRGB: return WGPUTextureFormat_RGBA8UnormSrgb;
		case texture_format::RGBAi8_Normalized: return WGPUTextureFormat_RGBA8Snorm;
		case texture_format::RGBAu8: return WGPUTextureFormat_RGBA8Uint;
		case texture_format::RGBAi8: return WGPUTextureFormat_RGBA8Sint;
		case texture_format::BGRAu8_Normalized: return WGPUTextureFormat_BGRA8Unorm;
		case texture_format::BGRAu8_NormalizedSRGB: return WGPUTextureFormat_BGRA8UnormSrgb;
		case texture_format::RGBu10Au2: return WGPUTextureFormat_RGB10A2Uint;
		case texture_format::RGBu10Au2_Normalized: return WGPUTextureFormat_RGB10A2Unorm;
		case texture_format::RGf11Bf10: return WGPUTextureFormat_RG11B10Ufloat;
		// c texture_format::RGB: returnase WGPUTextureFormat_RGB9E5Ufloat;
		case texture_format::RGf32: return WGPUTextureFormat_RG32Float;
		case texture_format::RGu32: return WGPUTextureFormat_RG32Uint;
		case texture_format::RGi32: return WGPUTextureFormat_RG32Sint;
		case texture_format::RGBAu16: return WGPUTextureFormat_RGBA16Uint;
		case texture_format::RGBAi16: return WGPUTextureFormat_RGBA16Sint;
		case texture_format::RGBAf16: return WGPUTextureFormat_RGBA16Float;
		case texture_format::RGBAf32: return WGPUTextureFormat_RGBA32Float;
		case texture_format::RGBAu32: return WGPUTextureFormat_RGBA32Uint;
		case texture_format::RGBAi32: return WGPUTextureFormat_RGBA32Sint;
		case texture_format::Stencil_u8: return WGPUTextureFormat_Stencil8;
		case texture_format::Depth_u16: return WGPUTextureFormat_Depth16Unorm;
		case texture_format::Depth_u24: return WGPUTextureFormat_Depth24Plus;
		case texture_format::Depth_u24Stencil_u8: return WGPUTextureFormat_Depth24PlusStencil8;
		case texture_format::Depth_f32: return WGPUTextureFormat_Depth32Float;
		case texture_format::Depth_f32Stencil_u8: return WGPUTextureFormat_Depth32FloatStencil8;
		case texture_format::BC1RGBA_Normalized: return WGPUTextureFormat_BC1RGBAUnorm;
		case texture_format::BC1RGBA_NormalizedSRGB: return WGPUTextureFormat_BC1RGBAUnormSrgb;
		case texture_format::BC2RGBA_Normalized: return WGPUTextureFormat_BC2RGBAUnorm;
		case texture_format::BC2RGBA_NormalizedSRGB: return WGPUTextureFormat_BC2RGBAUnormSrgb;
		case texture_format::BC3RGBA_Normalized: return WGPUTextureFormat_BC3RGBAUnorm;
		case texture_format::BC3RGBA_NormalizedSRGB: return WGPUTextureFormat_BC3RGBAUnormSrgb;
		case texture_format::BC4Ru_Normalized: return WGPUTextureFormat_BC4RUnorm;
		case texture_format::BC4Ri_Normalized: return WGPUTextureFormat_BC4RSnorm;
		case texture_format::BC5RGu_Normalized: return WGPUTextureFormat_BC5RGUnorm;
		case texture_format::BC5RGi_Normalized: return WGPUTextureFormat_BC5RGSnorm;
		case texture_format::BC6HRGBUfloat: return WGPUTextureFormat_BC6HRGBUfloat;
		case texture_format::BC6HRGBFloat: return WGPUTextureFormat_BC6HRGBFloat;
		case texture_format::BC7RGBA_Normalized: return WGPUTextureFormat_BC7RGBAUnorm;
		case texture_format::BC7RGBA_NormalizedSRGB: return WGPUTextureFormat_BC7RGBAUnormSrgb;
		case texture_format::ETC2RGB8_Normalized: return WGPUTextureFormat_ETC2RGB8Unorm;
		case texture_format::ETC2RGB8_NormalizedSRGB: return WGPUTextureFormat_ETC2RGB8UnormSrgb;
		case texture_format::ETC2RGB8A1_Normalized: return WGPUTextureFormat_ETC2RGB8A1Unorm;
		case texture_format::ETC2RGB8A1_NormalizedSRGB: return WGPUTextureFormat_ETC2RGB8A1UnormSrgb;
		case texture_format::ETC2RGBA8_Normalized: return WGPUTextureFormat_ETC2RGBA8Unorm;
		case texture_format::ETC2RGBA8_NormalizedSRGB: return WGPUTextureFormat_ETC2RGBA8UnormSrgb;
		case texture_format::EACRu11_Normalized: return WGPUTextureFormat_EACR11Unorm;
		case texture_format::EACRi11_Normalized: return WGPUTextureFormat_EACR11Snorm;
		case texture_format::EACRGu11_Normalized: return WGPUTextureFormat_EACRG11Unorm;
		case texture_format::EACRGi11_Normalized: return WGPUTextureFormat_EACRG11Snorm;
		case texture_format::ASTC4x4_Normalized: return WGPUTextureFormat_ASTC4x4Unorm;
		case texture_format::ASTC4x4_NormalizedSRGB: return WGPUTextureFormat_ASTC4x4UnormSrgb;
		case texture_format::ASTC5x4_Normalized: return WGPUTextureFormat_ASTC5x4Unorm;
		case texture_format::ASTC5x4_NormalizedSRGB: return WGPUTextureFormat_ASTC5x4UnormSrgb;
		case texture_format::ASTC5x5_Normalized: return WGPUTextureFormat_ASTC5x5Unorm;
		case texture_format::ASTC5x5_NormalizedSRGB: return WGPUTextureFormat_ASTC5x5UnormSrgb;
		case texture_format::ASTC6x5_Normalized: return WGPUTextureFormat_ASTC6x5Unorm;
		case texture_format::ASTC6x5_NormalizedSRGB: return WGPUTextureFormat_ASTC6x5UnormSrgb;
		case texture_format::ASTC6x6_Normalized: return WGPUTextureFormat_ASTC6x6Unorm;
		case texture_format::ASTC6x6_NormalizedSRGB: return WGPUTextureFormat_ASTC6x6UnormSrgb;
		case texture_format::ASTC8x5_Normalized: return WGPUTextureFormat_ASTC8x5Unorm;
		case texture_format::ASTC8x5_NormalizedSRGB: return WGPUTextureFormat_ASTC8x5UnormSrgb;
		case texture_format::ASTC8x6_Normalized: return WGPUTextureFormat_ASTC8x6Unorm;
		case texture_format::ASTC8x6_NormalizedSRGB: return WGPUTextureFormat_ASTC8x6UnormSrgb;
		case texture_format::ASTC8x8_Normalized: return WGPUTextureFormat_ASTC8x8Unorm;
		case texture_format::ASTC8x8_NormalizedSRGB: return WGPUTextureFormat_ASTC8x8UnormSrgb;
		case texture_format::ASTC10x5_Normalized: return WGPUTextureFormat_ASTC10x5Unorm;
		case texture_format::ASTC10x5_NormalizedSRGB: return WGPUTextureFormat_ASTC10x5UnormSrgb;
		case texture_format::ASTC10x6_Normalized: return WGPUTextureFormat_ASTC10x6Unorm;
		case texture_format::ASTC10x6_NormalizedSRGB: return WGPUTextureFormat_ASTC10x6UnormSrgb;
		case texture_format::ASTC10x8_Normalized: return WGPUTextureFormat_ASTC10x8Unorm;
		case texture_format::ASTC10x8_NormalizedSRGB: return WGPUTextureFormat_ASTC10x8UnormSrgb;
		case texture_format::ASTC10x10_Normalized: return WGPUTextureFormat_ASTC10x10Unorm;
		case texture_format::ASTC10x10_NormalizedSRGB: return WGPUTextureFormat_ASTC10x10UnormSrgb;
		case texture_format::ASTC12x10_Normalized: return WGPUTextureFormat_ASTC12x10Unorm;
		case texture_format::ASTC12x10_NormalizedSRGB: return WGPUTextureFormat_ASTC12x10UnormSrgb;
		case texture_format::ASTC12x12_Normalized: return WGPUTextureFormat_ASTC12x12Unorm;
		case texture_format::ASTC12x12_NormalizedSRGB: return WGPUTextureFormat_ASTC12x12UnormSrgb;
		case texture_format::Ru16_Normalized: return WGPUTextureFormat_R16Unorm;
		case texture_format::RGu16_Normalized: return WGPUTextureFormat_RG16Unorm;
		case texture_format::RGBAu16_Normalized: return WGPUTextureFormat_RGBA16Unorm;
		case texture_format::Ri16_Normalized: return WGPUTextureFormat_R16Snorm;
		case texture_format::RGi16_Normalized: return WGPUTextureFormat_RG16Snorm;
		case texture_format::RGBAi16_Normalized: return WGPUTextureFormat_RGBA16Snorm;
		case texture_format::R8BG8Biplanar420_Normalized: return WGPUTextureFormat_R8BG8Biplanar420Unorm;
		case texture_format::R10X6BG10X6Biplanar420_Normalized: return WGPUTextureFormat_R10X6BG10X6Biplanar420Unorm;
		case texture_format::R8BG8A8Triplanar420_Normalized: return WGPUTextureFormat_R8BG8A8Triplanar420Unorm;
		case texture_format::R8BG8Biplanar422_Normalized: return WGPUTextureFormat_R8BG8Biplanar422Unorm;
		case texture_format::R8BG8Biplanar444_Normalized: return WGPUTextureFormat_R8BG8Biplanar444Unorm;
		case texture_format::R10X6BG10X6Biplanar422_Normalized: return WGPUTextureFormat_R10X6BG10X6Biplanar422Unorm;
		case texture_format::R10X6BG10X6Biplanar444_Normalized: return WGPUTextureFormat_R10X6BG10X6Biplanar444Unorm;
		default:
			STYLIZER_API_THROW(std::string("Failed to find texture format: ") + std::string(magic_enum::enum_name(format)));
		}
	}

	inline std::string format_to_string(WGPUTextureFormat format) {
		switch (format) {
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
		default:
			STYLIZER_API_THROW(std::string("Failed to find texture format: ") + std::string(std::to_string(format)));
		}
	}
} // namespace stylizer::api::webgpu
