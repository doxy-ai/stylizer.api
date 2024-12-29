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

	#define STYLIZER_API_WGPU_TYPE "WGP"
	constexpr static const std::string_view type_string = STYLIZER_API_WGPU_TYPE;

	template<typename Tto, typename Tfrom>
	inline Tto& confirm_wgpu_type(Tfrom& ref) {
		auto& res = ref.template as<Tto>();
		assert(res.type == type_string);
		return res;
	}
	template<typename Tto, typename Tfrom>
	inline const Tto& confirm_wgpu_type(const Tfrom& ref) {
		auto& res = const_cast<Tfrom&>(ref).template as<Tto>();
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
		throw error("Unknown Alpha Mode: " + std::string(magic_enum::enum_name(mode)));
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
		throw error("Unknown Texture Format: " + std::string(magic_enum::enum_name(format)));
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
			default: throw error("Unknown Texture Format");
		}
	}

	inline wgpu::TextureUsageFlags to_wgpu_texture(usage usage) {
		wgpu::TextureUsageFlags out = {};
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
			throw error("Invalid Texture Usage(s): " + std::string(magic_enum::enum_flags_name(usage)));
		return out;
	}
	inline wgpu::BufferUsageFlags to_wgpu_buffer(usage usage) {
		wgpu::BufferUsageFlags out = {};
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
			throw error("Invalid Buffer Usage(s): " + std::string(magic_enum::enum_flags_name(usage)));
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
		throw error("Unknown Comparison Function: " + std::string(magic_enum::enum_name(func)));
	}

	inline wgpu::PresentMode to_wgpu(surface::present_mode mode) {
		switch(mode){
			case surface::present_mode::Fifo: return wgpu::PresentMode::Fifo;
			case surface::present_mode::FifoRelaxed: return wgpu::PresentMode::FifoRelaxed;
			case surface::present_mode::Immediate: return wgpu::PresentMode::Immediate;
			case surface::present_mode::Mailbox: return wgpu::PresentMode::Mailbox;
		}
		throw error("Unknown Present Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::AddressMode to_wgpu(texture::address_mode mode) {
		switch(mode){
			case texture::address_mode::Repeat: return wgpu::AddressMode::Repeat;
			case texture::address_mode::MirrorRepeat: return wgpu::AddressMode::MirrorRepeat;
			case texture::address_mode::ClampToEdge: return wgpu::AddressMode::ClampToEdge;
		}
		throw error("Unknown Address Mode: " + std::string(magic_enum::enum_name(mode)));
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
		throw error("Unknown Primitive Topology: " + std::string(magic_enum::enum_name(topology)));
	}

	inline wgpu::CullMode to_wgpu(enum render_pipeline::config::cull_mode mode) {
		switch(mode){
			case render_pipeline::config::cull_mode::Back: return wgpu::CullMode::Back;
			case render_pipeline::config::cull_mode::Front: return wgpu::CullMode::Front;
			// case render_pipeline::config::cull_mode::Both: throw
			case render_pipeline::config::cull_mode::Neither: return wgpu::CullMode::None;
		}
		throw error("Unknown Cull Mode: " + std::string(magic_enum::enum_name(mode)));
	}

	inline wgpu::BlendOperation to_wgpu(enum blend_state::operation op) {
		switch(op){
			case blend_state::operation::Add: return wgpu::BlendOperation::Add;
			case blend_state::operation::Subtract: return wgpu::BlendOperation::Subtract;
			case blend_state::operation::ReverseSubtract: return wgpu::BlendOperation::ReverseSubtract;
			case blend_state::operation::Min: return wgpu::BlendOperation::Min;
			case blend_state::operation::Max: return wgpu::BlendOperation::Max;
		}
		throw error("Unknown Blend Operation: " + std::string(magic_enum::enum_name(op)));
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
		throw error("Unknown Blend Factor: " + std::string(magic_enum::enum_name(factor)));
	}
}