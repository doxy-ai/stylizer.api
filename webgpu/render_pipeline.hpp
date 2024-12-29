#pragma once

#include "render_pass.hpp"
#include "shader.hpp"

namespace stylizer::api::webgpu {
	struct render_pipeline: public api::render_pipeline { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pipeline);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::RenderPipeline pipeline = nullptr;

		render_pipeline(render_pipeline&& o) { *this = std::move(o); }
		render_pipeline& operator=(render_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() const override { return pipeline; }

		static render_pipeline create(api::device& device_, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline") {
			static constexpr auto format_stride = [](enum render_pipeline::config::vertex_buffer_layout::attribute::format format) -> size_t {
				using fmt = enum render_pipeline::config::vertex_buffer_layout::attribute::format;
				switch(format){
				case fmt::f32x1: return sizeof(float) * 1;
				case fmt::f32x2: return sizeof(float) * 2;
				case fmt::f32x3: return sizeof(float) * 3;
				case fmt::f32x4: return sizeof(float) * 4;
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
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			auto& vertex = entry_points.at(shader::stage::Vertex);

			std::vector<WGPUVertexAttribute> vertex_attributes;
			std::vector<WGPUVertexBufferLayout> vertex_buffers; vertex_buffers.reserve(config.vertex_buffers.size());
			size_t shader_location = 0;
			for(auto& layout: config.vertex_buffers) {
				size_t starting_attribute = vertex_attributes.size();
				size_t stride = 0;
				for(auto& attribute: layout.attributes) {
					size_t offset = attribute.offset_override.value_or(stride);
					vertex_attributes.emplace_back(to_wgpu(attribute.format), offset, attribute.shader_location_override.value_or(shader_location));
					stride = std::max(stride, format_stride(attribute.format) + offset);
					shader_location++;
				}

				vertex_buffers.emplace_back(
					layout.stride_override.value_or(stride),
					layout.per_instance ? wgpu::VertexStepMode::Instance : wgpu::VertexStepMode::Vertex,
					vertex_attributes.size() - starting_attribute,
					(WGPUVertexAttribute*)starting_attribute // NOTE: saving the attribute start index
				);
			}
			for(auto& buffer: vertex_buffers) // NOTE: Need a secondary pass to set attribute pointers to account for the vector resizing!
				buffer.attributes = vertex_attributes.data() + (size_t)buffer.attributes;

			WGPUDepthStencilState depth_state{};
			if(depth_attachment) {
				assert(depth_attachment->texture || depth_attachment->view || depth_attachment->texture_format != texture_format::Undefined);
				depth_state = WGPUDepthStencilState{
					.format = to_wgpu(depth_attachment->texture ? depth_attachment->texture->texture_format()
						: depth_attachment->view ? (depth_attachment->view->texture().texture_format()) : depth_attachment->texture_format),
					.depthWriteEnabled = depth_attachment->should_store_depth && !depth_attachment->depth_readonly,
					.depthCompare = to_wgpu(depth_attachment->depth_comparison_function),
					.stencilFront = wgpu::StencilFaceState{wgpu::Default},
					.stencilBack = wgpu::StencilFaceState{wgpu::Default},
					.stencilReadMask = 0,
					.stencilWriteMask = 0,
					.depthBias = static_cast<int32_t>(depth_attachment->depth_bias),
					.depthBiasSlopeScale = depth_attachment->depth_bias_slope_scale,
					.depthBiasClamp = depth_attachment->depth_bias_clamp
				};
			}

			bool use_fragment_state = entry_points.contains(shader::stage::Fragment);
			WGPUFragmentState fragment_state{};
			std::vector<WGPUBlendState> blend_states;
			std::vector<WGPUColorTargetState> color_targets;
			if(use_fragment_state) {
				blend_states.reserve(color_attachments.size());
				color_targets.reserve(color_attachments.size());
				for(auto& target: color_attachments) {
					assert(depth_attachment->texture || depth_attachment->view || depth_attachment->texture_format != texture_format::Undefined);

					auto color_blend = target.color_blend_state.value_or(blend_state{});
					auto alpha_blend = target.alpha_blend_state.value_or(blend_state{});
					bool should_blend = target.color_blend_state || target.alpha_blend_state;
					if(should_blend)
						blend_states.emplace_back(WGPUBlendComponent{
							.operation = to_wgpu(color_blend.operation),
							.srcFactor = to_wgpu(color_blend.source_factor),
							.dstFactor = to_wgpu(color_blend.destination_factor)
						}, WGPUBlendComponent{
							.operation = to_wgpu(alpha_blend.operation),
							.srcFactor = to_wgpu(alpha_blend.source_factor),
							.dstFactor = to_wgpu(alpha_blend.destination_factor)
						});

					color_targets.emplace_back(WGPUColorTargetState{
						.format = to_wgpu(target.texture ? target.texture->texture_format()
							: target.view ? (target.view->texture().texture_format()) : target.texture_format),
						.blend = should_blend ? &blend_states.back() : nullptr,
						.writeMask = wgpu::ColorWriteMask::All, // TODO: Expose write mask?
					});
				}

				auto& fragment = entry_points.at(shader::stage::Fragment);
				fragment_state = {
					.module = confirm_wgpu_type<webgpu::shader>(*fragment.shader).module,
					.entryPoint = cstring_from_view(fragment.entry_point_name),
					.targetCount = color_targets.size(),
					.targets = color_targets.data()
				};
			}

			render_pipeline out;
			out.pipeline = device.device_.createRenderPipeline(WGPURenderPipelineDescriptor{
				.label = cstring_from_view(label),
				.layout = nullptr,
				.vertex = {
					.module = confirm_wgpu_type<webgpu::shader>(*vertex.shader).module,
					.entryPoint = cstring_from_view(vertex.entry_point_name),
					.bufferCount = vertex_buffers.size(),
					.buffers = vertex_buffers.data()
				},
				.primitive = {
					.topology = to_wgpu(config.primitive_topology),
					.stripIndexFormat = topology_is_strip(config.primitive_topology) ? config.u16_indices ? wgpu::IndexFormat::Uint16 : wgpu::IndexFormat::Uint32 : wgpu::IndexFormat::Undefined,
					.frontFace = config.winding_order_clockwise ? wgpu::FrontFace::CW : wgpu::FrontFace::CCW,
					.cullMode = to_wgpu(config.cull_mode)
				},
				.depthStencil = depth_attachment ? &depth_state : nullptr,
				.multisample = {
					.count = 1,
					.mask = ~0u,
					.alphaToCoverageEnabled = false,
				},
				.fragment = use_fragment_state ? &fragment_state : nullptr
			});
			return out;
		}

		static render_pipeline create_from_compatible_render_pass(api::device& device, const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline") {
			auto& render_pass = confirm_wgpu_type<webgpu::render_pass>(compatible_render_pass);
			return create(device, entry_points, render_pass.color_attachments, render_pass.depth_attachment, config, label);
		}

		webgpu::bind_group create_bind_group(api::device& device_, size_t index, std::span<const bind_group::binding> bindings) {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			return webgpu::bind_group::internal_create(device, index, pipeline.getBindGroupLayout(index), bindings);
		}
		api::bind_group& create_bind_group(temporary_return_t, api::device& device, size_t index, std::span<const bind_group::binding> bindings) override {
			static webgpu::bind_group group_;
			return group_ = create_bind_group(device, index, bindings);
		}

		void release() override {
			if(pipeline) std::exchange(pipeline, nullptr).release();
		}
	};
	static_assert(render_pipeline_concept<render_pipeline>);
}