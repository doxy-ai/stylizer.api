#include "common.hpp"

namespace stylizer::api::webgpu {

	WGPUVertexFormat to_webgpu(enum render_pipeline::config::vertex_buffer_layout::attribute::format format) {
		switch(format) {
		case render_pipeline::config::vertex_buffer_layout::attribute::format::f32x1: return WGPUVertexFormat_Float32;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::f32x2: return WGPUVertexFormat_Float32x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::f32x3: return WGPUVertexFormat_Float32x3;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::f32x4: return WGPUVertexFormat_Float32x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::f16x2: return WGPUVertexFormat_Float16x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::f16x4: return WGPUVertexFormat_Float16x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i32x1: return WGPUVertexFormat_Sint32;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i32x2: return WGPUVertexFormat_Sint32x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i32x3: return WGPUVertexFormat_Sint32x3;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i32x4: return WGPUVertexFormat_Sint32x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i16x2: return WGPUVertexFormat_Sint16x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i16x4: return WGPUVertexFormat_Sint16x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i16x2_normalized: return WGPUVertexFormat_Snorm16x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i16x4_normalized: return WGPUVertexFormat_Snorm16x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i8x2: return WGPUVertexFormat_Sint8x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i8x4: return WGPUVertexFormat_Sint8x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i8x2_normalized: return WGPUVertexFormat_Snorm8x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::i8x4_normalized: return WGPUVertexFormat_Snorm8x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u32x1: return WGPUVertexFormat_Uint32;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u32x2: return WGPUVertexFormat_Uint32x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u32x3: return WGPUVertexFormat_Uint32x3;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u32x4: return WGPUVertexFormat_Uint32x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u16x2: return WGPUVertexFormat_Uint16x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u16x4: return WGPUVertexFormat_Uint16x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u16x2_normalized: return WGPUVertexFormat_Unorm16x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u16x4_normalized: return WGPUVertexFormat_Unorm16x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u8x2: return WGPUVertexFormat_Uint8x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u8x4: return WGPUVertexFormat_Uint8x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u8x2_normalized: return WGPUVertexFormat_Unorm8x2;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u8x4_normalized: return WGPUVertexFormat_Unorm8x4;
		case render_pipeline::config::vertex_buffer_layout::attribute::format::u10_10_10_2_normalized: return WGPUVertexFormat_Unorm10_10_10_2;
		default:
			STYLIZER_API_THROW(std::string("Failed to find attribute format: ") + std::string(magic_enum::enum_name(format)));
		}
	}
	enum render_pipeline::config::vertex_buffer_layout::attribute::format from_webgpu(WGPUVertexFormat format) {
		switch(format) {
		case WGPUVertexFormat_Float32: return render_pipeline::config::vertex_buffer_layout::attribute::format::f32x1;
		case WGPUVertexFormat_Float32x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::f32x2;
		case WGPUVertexFormat_Float32x3: return render_pipeline::config::vertex_buffer_layout::attribute::format::f32x3;
		case WGPUVertexFormat_Float32x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::f32x4;
		case WGPUVertexFormat_Float16x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::f16x2;
		case WGPUVertexFormat_Float16x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::f16x4;
		case WGPUVertexFormat_Sint32: return render_pipeline::config::vertex_buffer_layout::attribute::format::i32x1;
		case WGPUVertexFormat_Sint32x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::i32x2;
		case WGPUVertexFormat_Sint32x3: return render_pipeline::config::vertex_buffer_layout::attribute::format::i32x3;
		case WGPUVertexFormat_Sint32x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::i32x4;
		case WGPUVertexFormat_Sint16x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::i16x2;
		case WGPUVertexFormat_Sint16x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::i16x4;
		case WGPUVertexFormat_Snorm16x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::i16x2_normalized;
		case WGPUVertexFormat_Snorm16x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::i16x4_normalized;
		case WGPUVertexFormat_Sint8x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::i8x2;
		case WGPUVertexFormat_Sint8x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::i8x4;
		case WGPUVertexFormat_Snorm8x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::i8x2_normalized;
		case WGPUVertexFormat_Snorm8x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::i8x4_normalized;
		case WGPUVertexFormat_Uint32: return render_pipeline::config::vertex_buffer_layout::attribute::format::u32x1;
		case WGPUVertexFormat_Uint32x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::u32x2;
		case WGPUVertexFormat_Uint32x3: return render_pipeline::config::vertex_buffer_layout::attribute::format::u32x3;
		case WGPUVertexFormat_Uint32x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::u32x4;
		case WGPUVertexFormat_Uint16x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::u16x2;
		case WGPUVertexFormat_Uint16x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::u16x4;
		case WGPUVertexFormat_Unorm16x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::u16x2_normalized;
		case WGPUVertexFormat_Unorm16x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::u16x4_normalized;
		case WGPUVertexFormat_Uint8x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::u8x2;
		case WGPUVertexFormat_Uint8x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::u8x4;
		case WGPUVertexFormat_Unorm8x2: return render_pipeline::config::vertex_buffer_layout::attribute::format::u8x2_normalized;
		case WGPUVertexFormat_Unorm8x4: return render_pipeline::config::vertex_buffer_layout::attribute::format::u8x4_normalized;
		case WGPUVertexFormat_Unorm10_10_10_2: return render_pipeline::config::vertex_buffer_layout::attribute::format::u10_10_10_2_normalized;
		default:
			STYLIZER_API_THROW(std::string("Failed to find attribute format: ") + std::string(magic_enum::enum_name(format)));
		}
	}

	WGPUStencilOperation to_webgpu(depth_stencil_attachment::stencil_config::face_state::operation op) {
		switch(op) {
		case depth_stencil_attachment::stencil_config::face_state::operation::Undefined: return WGPUStencilOperation_Undefined;
		case depth_stencil_attachment::stencil_config::face_state::operation::Keep: return WGPUStencilOperation_Keep;
		case depth_stencil_attachment::stencil_config::face_state::operation::Zero: return WGPUStencilOperation_Zero;
		case depth_stencil_attachment::stencil_config::face_state::operation::Replace: return WGPUStencilOperation_Replace;
		case depth_stencil_attachment::stencil_config::face_state::operation::Invert: return WGPUStencilOperation_Invert;
		case depth_stencil_attachment::stencil_config::face_state::operation::IncrementClamp: return WGPUStencilOperation_IncrementClamp;
		case depth_stencil_attachment::stencil_config::face_state::operation::DecrementClamp: return WGPUStencilOperation_DecrementClamp;
		case depth_stencil_attachment::stencil_config::face_state::operation::IncrementWrap: return WGPUStencilOperation_IncrementWrap;
		case depth_stencil_attachment::stencil_config::face_state::operation::DecrementWrap: return WGPUStencilOperation_DecrementWrap;
		default:
			STYLIZER_API_THROW(std::string("Failed to find face operation: ") + std::string(magic_enum::enum_name(op)));
		}
	}
	depth_stencil_attachment::stencil_config::face_state::operation from_webgpu(WGPUStencilOperation op) {
		switch(op) {
		case WGPUStencilOperation_Undefined: return depth_stencil_attachment::stencil_config::face_state::operation::Undefined;
		case WGPUStencilOperation_Keep: return depth_stencil_attachment::stencil_config::face_state::operation::Keep;
		case WGPUStencilOperation_Zero: return depth_stencil_attachment::stencil_config::face_state::operation::Zero;
		case WGPUStencilOperation_Replace: return depth_stencil_attachment::stencil_config::face_state::operation::Replace;
		case WGPUStencilOperation_Invert: return depth_stencil_attachment::stencil_config::face_state::operation::Invert;
		case WGPUStencilOperation_IncrementClamp: return depth_stencil_attachment::stencil_config::face_state::operation::IncrementClamp;
		case WGPUStencilOperation_DecrementClamp: return depth_stencil_attachment::stencil_config::face_state::operation::DecrementClamp;
		case WGPUStencilOperation_IncrementWrap: return depth_stencil_attachment::stencil_config::face_state::operation::IncrementWrap;
		case WGPUStencilOperation_DecrementWrap: return depth_stencil_attachment::stencil_config::face_state::operation::DecrementWrap;
		default:
			STYLIZER_API_THROW(std::string("Failed to find face operation: ") + std::string(magic_enum::enum_name(op)));
		}
	}

	WGPUBlendOperation to_webgpu(enum blend_state::operation op) {
		switch (op) {
		case blend_state::operation::Add: return WGPUBlendOperation_Add;
		case blend_state::operation::Subtract: return WGPUBlendOperation_Subtract;
		case blend_state::operation::ReverseSubtract: return WGPUBlendOperation_ReverseSubtract;
		case blend_state::operation::Min: return WGPUBlendOperation_Min;
		case blend_state::operation::Max: return WGPUBlendOperation_Max;
		default:
			STYLIZER_API_THROW(std::string("Failed to find blend operation: ") + std::string(magic_enum::enum_name(op)));
		}
	}
	enum blend_state::operation from_webgpu(WGPUBlendOperation op) {
		switch (op) {
		case WGPUBlendOperation_Add: return blend_state::operation::Add;
		case WGPUBlendOperation_Subtract: return blend_state::operation::Subtract;
		case WGPUBlendOperation_ReverseSubtract: return blend_state::operation::ReverseSubtract;
		case WGPUBlendOperation_Min: return blend_state::operation::Min;
		case WGPUBlendOperation_Max: return blend_state::operation::Max;
		default:
			STYLIZER_API_THROW(std::string("Failed to find blend operation: ") + std::string(magic_enum::enum_name(op)));
		}
	}

	WGPUBlendFactor to_webgpu(enum blend_state::factor op) {
		switch(op){
		case blend_state::factor::Zero: return WGPUBlendFactor_Zero;
		case blend_state::factor::One: return WGPUBlendFactor_One;
		case blend_state::factor::Source: return WGPUBlendFactor_Src;
		case blend_state::factor::OneMinusSource: return WGPUBlendFactor_OneMinusSrc;
		case blend_state::factor::SourceAlpha: return WGPUBlendFactor_SrcAlpha;
		case blend_state::factor::OneMinusSourceAlpha: return WGPUBlendFactor_OneMinusSrcAlpha;
		case blend_state::factor::Destination: return WGPUBlendFactor_Dst;
		case blend_state::factor::OneMinusDestination: return WGPUBlendFactor_OneMinusDst;
		case blend_state::factor::DestinationAlpha: return WGPUBlendFactor_DstAlpha;
		case blend_state::factor::OneMinusDestinationAlpha: return WGPUBlendFactor_OneMinusDstAlpha;
		case blend_state::factor::SourceAlphaSaturated: return WGPUBlendFactor_SrcAlphaSaturated;
		case blend_state::factor::Constant: return WGPUBlendFactor_Constant;
		case blend_state::factor::OneMinusConstant: return WGPUBlendFactor_OneMinusConstant;
		default:
			STYLIZER_API_THROW(std::string("Failed to find blend factor: ") + std::string(magic_enum::enum_name(op)));
		}
	}
	enum blend_state::factor from_webgpu(WGPUBlendFactor op) {
		switch(op){
		case WGPUBlendFactor_Zero: return blend_state::factor::Zero;
		case WGPUBlendFactor_One: return blend_state::factor::One;
		case WGPUBlendFactor_Src: return blend_state::factor::Source;
		case WGPUBlendFactor_OneMinusSrc: return blend_state::factor::OneMinusSource;
		case WGPUBlendFactor_SrcAlpha: return blend_state::factor::SourceAlpha;
		case WGPUBlendFactor_OneMinusSrcAlpha: return blend_state::factor::OneMinusSourceAlpha;
		case WGPUBlendFactor_Dst: return blend_state::factor::Destination;
		case WGPUBlendFactor_OneMinusDst: return blend_state::factor::OneMinusDestination;
		case WGPUBlendFactor_DstAlpha: return blend_state::factor::DestinationAlpha;
		case WGPUBlendFactor_OneMinusDstAlpha: return blend_state::factor::OneMinusDestinationAlpha;
		case WGPUBlendFactor_SrcAlphaSaturated: return blend_state::factor::SourceAlphaSaturated;
		case WGPUBlendFactor_Constant: return blend_state::factor::Constant;
		case WGPUBlendFactor_OneMinusConstant: return blend_state::factor::OneMinusConstant;
		default:
			STYLIZER_API_THROW(std::string("Failed to find blend factor: ") + std::string(magic_enum::enum_name(op)));
		}
	}

	WGPUPrimitiveTopology to_webgpu(enum render_pipeline::config::primitive_topology topology) {
		switch (topology) {
		case render_pipeline::config::primitive_topology::PointList: return WGPUPrimitiveTopology_PointList;
		case render_pipeline::config::primitive_topology::LineList: return WGPUPrimitiveTopology_LineList;
		case render_pipeline::config::primitive_topology::LineStrip: return WGPUPrimitiveTopology_LineStrip;
		case render_pipeline::config::primitive_topology::TriangleList: return WGPUPrimitiveTopology_TriangleList;
		case render_pipeline::config::primitive_topology::TriangleStrip: return WGPUPrimitiveTopology_TriangleStrip;
		default:
			STYLIZER_API_THROW(std::string("Failed to find primitive topology: ") + std::string(magic_enum::enum_name(topology)));
		}
	}
	enum render_pipeline::config::primitive_topology from_webgpu(WGPUPrimitiveTopology topology) {
		switch (topology) {
		case WGPUPrimitiveTopology_PointList: return render_pipeline::config::primitive_topology::PointList;
		case WGPUPrimitiveTopology_LineList: return render_pipeline::config::primitive_topology::LineList;
		case WGPUPrimitiveTopology_LineStrip: return render_pipeline::config::primitive_topology::LineStrip;
		case WGPUPrimitiveTopology_TriangleList: return render_pipeline::config::primitive_topology::TriangleList;
		case WGPUPrimitiveTopology_TriangleStrip: return render_pipeline::config::primitive_topology::TriangleStrip;
		default:
			STYLIZER_API_THROW(std::string("Failed to find primitive topology: ") + std::string(magic_enum::enum_name(topology)));
		}
	}

	WGPUCullMode to_webgpu(enum render_pipeline::config::cull_mode mode){
		switch (mode) {
		case render_pipeline::config::cull_mode::Neither: return WGPUCullMode_None;
		case render_pipeline::config::cull_mode::Front: return WGPUCullMode_Front;
		case render_pipeline::config::cull_mode::Back: return WGPUCullMode_Back;
		case render_pipeline::config::cull_mode::Both: return WGPUCullMode_Undefined;
		default:
			STYLIZER_API_THROW(std::string("Failed to find culling mode: ") + std::string(magic_enum::enum_name(mode)));
		}
	}
	enum render_pipeline::config::cull_mode from_webgpu(WGPUCullMode mode){
		switch (mode) {
		case WGPUCullMode_None: return render_pipeline::config::cull_mode::Neither;
		case WGPUCullMode_Front: return render_pipeline::config::cull_mode::Front;
		case WGPUCullMode_Back: return render_pipeline::config::cull_mode::Back;
		case WGPUCullMode_Undefined: return render_pipeline::config::cull_mode::Both;
		default:
			STYLIZER_API_THROW(std::string("Failed to find culling mode: ") + std::string(magic_enum::enum_name(mode)));
		}
	}

	render_pipeline render_pipeline::create(api::device& device_, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments /* = {} */, const std::optional<depth_stencil_attachment>& depth_attachment /* = {} */, const render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Graphics Pipeline" */) {
		static constexpr auto format_stride = [](enum render_pipeline::config::vertex_buffer_layout::attribute::format format) -> size_t {
			using fmt = STYLIZER_ENUM_CLASS render_pipeline::config::vertex_buffer_layout::attribute::format;
			switch(format){
			case fmt::f32x1: return sizeof(float) * 1;
			case fmt::f32x2: return sizeof(float) * 2;
			case fmt::f32x3: return sizeof(float) * 4; // Note: GPUs expect them to be aligned on 4
			case fmt::f32x4: return sizeof(float) * 4;
			case fmt::f16x2: return sizeof(uint16_t) * 2;
			case fmt::f16x4: return sizeof(uint16_t) * 4;
			case fmt::i32x1: return sizeof(int32_t) * 1;
			case fmt::i32x2: return sizeof(int32_t) * 2;
			case fmt::i32x3: return sizeof(int32_t) * 4; // Note: GPUs expect them to be aligned on 4
			case fmt::i32x4: return sizeof(int32_t) * 4;
			case fmt::i16x2: return sizeof(int16_t) * 2;
			case fmt::i16x4: return sizeof(int16_t) * 4;
			case fmt::i16x2_normalized: return sizeof(int16_t) * 2;
			case fmt::i16x4_normalized: return sizeof(int16_t) * 4;
			case fmt::i8x2: return sizeof(int8_t) * 2;
			case fmt::i8x4: return sizeof(int8_t) * 4;
			case fmt::i8x2_normalized: return sizeof(int8_t) * 2;
			case fmt::i8x4_normalized: return sizeof(int8_t) * 4;
			case fmt::u32x1: return sizeof(int32_t) * 1;
			case fmt::u32x2: return sizeof(int32_t) * 2;
			case fmt::u32x3: return sizeof(int32_t) * 4; // Note: GPUs expect them to be aligned on 4
			case fmt::u32x4: return sizeof(int32_t) * 4;
			case fmt::u16x2: return sizeof(int16_t) * 2;
			case fmt::u16x4: return sizeof(int16_t) * 4;
			case fmt::u16x2_normalized: return sizeof(int16_t) * 2;
			case fmt::u16x4_normalized: return sizeof(int16_t) * 4;
			case fmt::u8x2: return sizeof(int8_t) * 2;
			case fmt::u8x4: return sizeof(int8_t) * 4;
			case fmt::u8x2_normalized: return sizeof(int8_t) * 2;
			case fmt::u8x4_normalized: return sizeof(int8_t) * 4;
			default: return 0;
			}
		};

		static constexpr auto topology_is_strip = [](enum render_pipeline::config::primitive_topology topology) -> bool {
			switch(topology){
				case render_pipeline::config::primitive_topology::LineStrip:
				case render_pipeline::config::primitive_topology::TriangleStrip: return true;
				default: return false;
			}
		};

		assert(entry_points.contains(shader::stage::Vertex));
		assert(entry_points.at(shader::stage::Vertex).shader);
		assert(std::abs(config.line_pixel_width - 1) < std::numeric_limits<float>::epsilon()); // TODO: WEBGPU Doesn't support line width?
		auto& device = confirm_webgpu_type<webgpu::device>(device_);

		auto& vertex = entry_points.at(shader::stage::Vertex);

		std::vector<WGPUVertexAttribute> vertex_attributes; vertex_attributes.reserve(config.vertex_buffers.size());
		std::vector<WGPUVertexBufferLayout> vertex_buffers; vertex_buffers.reserve(config.vertex_buffers.size());
		size_t shader_location = 0;
		for(auto& layout: config.vertex_buffers) {
			size_t starting_attribute = vertex_attributes.size();
			size_t stride = 0;
			for(auto& attribute: layout.attributes) {\
				uint64_t offset = attribute.offset_override.value_or(stride);
				vertex_attributes.emplace_back(nullptr, to_webgpu(attribute.format), offset, (uint32_t)attribute.shader_location_override.value_or(shader_location));
				stride = std::max(stride, format_stride(attribute.format) + offset);
				shader_location++;
			}

			vertex_buffers.emplace_back(
				nullptr,
				layout.per_instance ? WGPUVertexStepMode_Instance : WGPUVertexStepMode_Vertex,
				layout.stride_override.value_or(stride),
				vertex_attributes.size() - starting_attribute,
				(WGPUVertexAttribute*)starting_attribute // NOTE: saving the attribute start index
			);
		}
		for(auto& buffer: vertex_buffers) // NOTE: Need a secondary pass to set attribute pointers to account for the vector resizing!
			buffer.attributes = vertex_attributes.data() + (size_t)buffer.attributes;

		WGPUDepthStencilState depth_state = WGPU_DEPTH_STENCIL_STATE_INIT;
		if(depth_attachment) {
			assert(depth_attachment->texture || depth_attachment->view || depth_attachment->texture_format != texture_format::Undefined);
			depth_state.format = to_webgpu(depth_attachment->texture ? depth_attachment->texture->texture_format()
				: depth_attachment->view ? (depth_attachment->view->texture().texture_format()) : depth_attachment->texture_format);
			depth_state.depthWriteEnabled = to_webgpu(depth_attachment->should_store_depth && !depth_attachment->depth_readonly);
			depth_state.depthCompare = to_webgpu(depth_attachment->depth_comparison_function);
			if(depth_attachment->stencil.has_value())
				depth_state.stencilFront = WGPUStencilFaceState {
					.compare = to_webgpu(depth_attachment->stencil->front.compare),
					.failOp = to_webgpu(depth_attachment->stencil->front.fail_operation),
					.depthFailOp = to_webgpu(depth_attachment->stencil->front.depth_fail_operation),
					.passOp = to_webgpu(depth_attachment->stencil->front.pass_operation)
				};
			// else depth_state.stencilFront = WGPU_STENCIL_FACE_STATE_INIT;
			if(depth_attachment->stencil.has_value())
				depth_state.stencilBack = WGPUStencilFaceState {
					.compare = to_webgpu(depth_attachment->stencil->back.compare),
					.failOp = to_webgpu(depth_attachment->stencil->back.fail_operation),
					.depthFailOp = to_webgpu(depth_attachment->stencil->back.depth_fail_operation),
					.passOp = to_webgpu(depth_attachment->stencil->back.pass_operation)
				};
			// else depth_state.stencilBack = WGPU_STENCIL_FACE_STATE_INIT;
			depth_state.stencilReadMask = depth_attachment->stencil.has_value() ? depth_attachment->stencil->stencilReadMask : 0;
			depth_state.stencilWriteMask = depth_attachment->stencil.has_value() ? depth_attachment->stencil->stencilWriteMask : 0;
			depth_state.depthBias = static_cast<int32_t>(depth_attachment->depth_bias);
			depth_state.depthBiasSlopeScale = depth_attachment->depth_bias_slope_scale;
			depth_state.depthBiasClamp = depth_attachment->depth_bias_clamp;
		}

		bool use_fragment_state = entry_points.contains(shader::stage::Fragment);
		WGPUFragmentState fragment_state{};
		std::vector<WGPUBlendState> blend_states;
		std::vector<WGPUColorTargetState> color_targets;
		if(use_fragment_state) {
			blend_states.reserve(color_attachments.size());
			color_targets.reserve(color_attachments.size());
			for(auto& target: color_attachments) {
				assert(target.texture || target.view || target.texture_format != texture_format::Undefined);

				auto color_blend = target.color_blend_state.value_or(blend_state{});
				auto alpha_blend = target.alpha_blend_state.value_or(blend_state{});
				bool should_blend = target.color_blend_state || target.alpha_blend_state;
				if(should_blend)
					blend_states.emplace_back(WGPUBlendComponent{
						.operation = to_webgpu(color_blend.operation),
						.srcFactor = to_webgpu(color_blend.source_factor),
						.dstFactor = to_webgpu(color_blend.destination_factor)
					}, WGPUBlendComponent{
						.operation = to_webgpu(alpha_blend.operation),
						.srcFactor = to_webgpu(alpha_blend.source_factor),
						.dstFactor = to_webgpu(alpha_blend.destination_factor)
					});

				color_targets.emplace_back(WGPUColorTargetState{
					.format = to_webgpu(target.texture ? target.texture->texture_format()
						: target.view ? (target.view->texture().texture_format()) : target.texture_format),
					.blend = should_blend ? &blend_states.back() : nullptr,
					.writeMask = WGPUColorWriteMask_All, // TODO: Expose write mask?
				});
			}

			auto& fragment = entry_points.at(shader::stage::Fragment);
			fragment_state = {
				.module = confirm_webgpu_type<webgpu::shader>(*fragment.shader).module,
				.entryPoint = to_webgpu(fragment.entry_point_name),
				.targetCount = color_targets.size(),
				.targets = color_targets.data()
			};
		}

		render_pipeline out;
		WGPURenderPipelineDescriptor d = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
		d.label = to_webgpu(label),
		d.layout = nullptr,
		d.vertex = {
			.module = confirm_webgpu_type<webgpu::shader>(*vertex.shader).module,
			.entryPoint = to_webgpu(vertex.entry_point_name),
			.bufferCount = vertex_buffers.size(),
			.buffers = vertex_buffers.data()
		},
		d.primitive = {
			.topology = to_webgpu(config.primitive_topology),
			.stripIndexFormat = topology_is_strip(config.primitive_topology) ? config.u16_indices ? WGPUIndexFormat_Uint16 : WGPUIndexFormat_Uint32 : WGPUIndexFormat_Undefined,
			.frontFace = config.winding_order_clockwise ? WGPUFrontFace_CW : WGPUFrontFace_CCW,
			.cullMode = to_webgpu(config.cull_mode)
		},
		d.depthStencil = depth_attachment ? &depth_state : nullptr,
		d.multisample = {
			.count = config.multisampling.count,
			.mask = config.multisampling.mask,
			.alphaToCoverageEnabled = config.multisampling.alpha_to_coverage,
		},
		d.fragment = use_fragment_state ? &fragment_state : nullptr;
		out.pipeline = wgpuDeviceCreateRenderPipeline(device.device_, &d);
		return out;
	}

	render_pipeline render_pipeline::create_from_compatible_render_pass(api::device& device, const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Graphics Pipeline" */) {
		auto& render_pass = confirm_webgpu_type<webgpu::render_pass>(compatible_render_pass);
		return create(device, entry_points, render_pass.color_attachments, render_pass.depth_attachment, config, label);
	}

	// NOTE: Defined in compute_pipeline.cpp
	bind_group internal_bind_group_create(webgpu::device& device, size_t index, WGPUBindGroupLayout layout, std::span<const bind_group::binding> bindings);

	webgpu::bind_group render_pipeline::create_bind_group(api::device& device_, size_t index, std::span<const bind_group::binding> bindings) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		return internal_bind_group_create(device, index, wgpuRenderPipelineGetBindGroupLayout(pipeline, index), bindings);
	}
	api::bind_group& render_pipeline::create_bind_group(temporary_return_t, api::device& device, size_t index, std::span<const bind_group::binding> bindings) {
		static webgpu::bind_group group_;
		return group_ = create_bind_group(device, index, bindings);
	}

	void render_pipeline::release(){
		if(pipeline) wgpuRenderPipelineRelease(std::exchange(pipeline, nullptr));
	}

	// static_assert(shader_concept<shader>);
} // namespace stylizer::api::webgpu