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

namespace stylizer::api::webgpu {

	#define STYLIZER_API_WGPU_TYPE "WGP"
	constexpr static std::string_view type_string = STYLIZER_API_WGPU_TYPE;

	template<typename Tto, typename Tfrom>
	inline Tto& confirm_wgpu_type(Tfrom& p) {
		auto& res = p.template as<Tto>();
		assert(res.type == type_string);
		return res;
	}

	inline wgpu::Instance get_instance() {
		static auto_release<wgpu::Instance> instance = wgpu::createInstance({});
		return instance;
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
		throw error("Uknown Alpha Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::TextureFormat to_wgpu(texture_format format) {
		switch(format){
			case texture_format::Depth24: return wgpu::TextureFormat::Depth24Plus;
			case texture_format::RGBA32: return wgpu::TextureFormat::RGBA32Float;
			case texture_format::Rf32: return wgpu::TextureFormat::R32Float;
			case texture_format::Ru8: return wgpu::TextureFormat::R8Unorm;
			case texture_format::RGBA8: return wgpu::TextureFormat::RGBA8Unorm;
			case texture_format::RGBA8_SRGB: return wgpu::TextureFormat::RGBA8UnormSrgb;
			case texture_format::BGRA8_SRGB: return wgpu::TextureFormat::BGRA8UnormSrgb;
		}
		throw error("Uknown Texture Format: " + std::string(magic_enum::enum_name(format)));
	}

	inline texture_format from_wgpu(wgpu::TextureFormat format) {
		switch(format) {
			case wgpu::TextureFormat::Depth24Plus: return texture_format::Depth24;
			case wgpu::TextureFormat::RGBA32Float: return texture_format::RGBA32;
			case wgpu::TextureFormat::R32Float: return texture_format::Rf32;
			case wgpu::TextureFormat::R8Unorm: return texture_format::Ru8;
			case wgpu::TextureFormat::RGBA8Unorm: return texture_format::RGBA8;
			case wgpu::TextureFormat::RGBA8UnormSrgb: return texture_format::RGBA8_SRGB;
			case wgpu::TextureFormat::BGRA8UnormSrgb: return texture_format::BGRA8_SRGB;
			default: throw error("Unkown Texture Format");
        }
	}

	inline wgpu::TextureUsageFlags to_wgpu_texture(usage usage) {
		wgpu::TextureUsageFlags out;
		if((usage & usage::RenderAttachment) > usage::Invalid)
			out |= wgpu::TextureUsage::RenderAttachment;
		return out;
	}

	inline wgpu::CompareFunction to_wgpu(comparison_function func) {
		switch(func){
			case comparison_function::Never: return wgpu::CompareFunction::Never;
			case comparison_function::Less: return wgpu::CompareFunction::Less;
			case comparison_function::LessEqual: return wgpu::CompareFunction::LessEqual;
			case comparison_function::Greater: return wgpu::CompareFunction::Greater;
			case comparison_function::GreaterEqual: return wgpu::CompareFunction::GreaterEqual;
			case comparison_function::Equal: return wgpu::CompareFunction::Equal;
			case comparison_function::NotEqual: return wgpu::CompareFunction::NotEqual;
			case comparison_function::Always: return wgpu::CompareFunction::Always;
		}
		throw error("Uknown Comparison Function: " + std::string(magic_enum::enum_name(func)));
	}

	inline wgpu::PresentMode to_wgpu(surface::present_mode mode) {
		switch(mode){
			case surface::present_mode::Fifo: return wgpu::PresentMode::Fifo;
			case surface::present_mode::FifoRelaxed: return wgpu::PresentMode::FifoRelaxed;
			case surface::present_mode::Immediate: return wgpu::PresentMode::Immediate;
			case surface::present_mode::Mailbox: return wgpu::PresentMode::Mailbox;
		}
		throw error("Uknown Present Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::AddressMode to_wgpu(texture::address_mode mode) {
		switch(mode){
			case texture::address_mode::Repeat: return wgpu::AddressMode::Repeat;
			case texture::address_mode::MirrorRepeat: return wgpu::AddressMode::MirrorRepeat;
			case texture::address_mode::ClampToEdge: return wgpu::AddressMode::ClampToEdge;
		}
        throw error("Uknown Address Mode: " + std::string(magic_enum::enum_name(mode)));
	}	
}