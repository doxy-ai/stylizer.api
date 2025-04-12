#include "device.hpp"
#include "shader.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "command_encoder.hpp"
#include "render_pass.hpp"
#include "compute_pipeline.hpp"
#include "render_pipeline.hpp"

namespace stylizer::api::webgpu {
	device device::create_default(const create_config& config) {
		device out;
		out.adapter = get_instance().requestAdapter(WGPURequestAdapterOptions{
			.compatibleSurface = config.compatible_surface ? confirm_wgpu_type<webgpu::surface>(*config.compatible_surface).surface_ : nullptr,
			.powerPreference = config.high_performance ? wgpu::PowerPreference::HighPerformance : wgpu::PowerPreference::LowPower,
		});
		WGPUFeatureName float32filterable = WGPUFeatureName_Float32Filterable;
		out.device_ = out.adapter.requestDevice(WGPUDeviceDescriptor{
			.label = "Stylizer Device",
			.requiredFeatureCount = 1,
			.requiredFeatures = &float32filterable,
			.requiredLimits = nullptr,
			.defaultQueue = { .label = "Stylizer Queue" },
		});
		out.error_callback = out.device_.setUncapturedErrorCallback([](wgpu::ErrorType type, char const * message){
			if(type <= wgpu::ErrorType::Validation)
				std::cerr << type << ": " << message << std::endl;
			else STYLIZER_API_THROW(message);
		});
		out.queue = out.device_.getQueue();
		return out;
	}

	webgpu::texture device::create_texture(const api::texture::create_config& config /* = {} */) {
		return webgpu::texture::create(*this, config);
	}
	api::texture& device::create_texture(temporary_return_t, const api::texture::create_config& config /* = {} */) {
		static webgpu::texture texture;
		return texture = create_texture(config);
	}

	webgpu::texture device::create_and_write_texture(std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config /* = {} */) {
		return webgpu::texture::create_and_write(*this, data, layout, config);
	}
	api::texture& device::create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config /* = {} */) {
		static webgpu::texture texture;
		return texture = create_and_write_texture(data, layout, config);
	}

	webgpu::buffer device::create_buffer(usage usage, size_t size, bool mapped_at_creation /* = false */, const std::string_view label /* = "Stylizer Buffer" */) {
		return webgpu::buffer::create(*this, usage, size, mapped_at_creation, label);
	}
	api::buffer& device::create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation/*  = false */, const std::string_view label /* = "Stylizer Buffer" */) {
		static webgpu::buffer buffer;
		return buffer = create_buffer(usage, size, mapped_at_creation, label);
	}
	webgpu::buffer device::create_and_write_buffer(usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		return webgpu::buffer::create_and_write(*this, usage, data, offset, label);
	}
	api::buffer& device::create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		static webgpu::buffer buffer;
		return buffer = create_and_write_buffer(usage, data, offset, label);
	}

	webgpu::shader device::create_shader_from_spirv(slcross::spirv_view spirv, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_spirv(*this, spirv, label);
	}
	api::shader& device::create_shader_from_spirv(temporary_return_t, slcross::spirv_view spirv, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader shader;
		return shader = create_shader_from_spirv(spirv, label);
	}

	webgpu::command_encoder device::create_command_encoder(bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		return webgpu::command_encoder::create(*this, one_shot, label);
	}
	api::command_encoder& device::create_command_encoder(temporary_return_t, bool one_shot /* = false */, const std::string_view label/*  = "Stylizer Command Encoder" */) {
		static webgpu::command_encoder encoder;
		return encoder = create_command_encoder(one_shot, label);
	}

	webgpu::render_pass device::create_render_pass(std::span<const api::render_pass::color_attachment> colors, optional<api::render_pass::depth_stencil_attachment> depth /* = {} */, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Render Pass" */) {
		return webgpu::render_pass::create(*this, colors, depth, one_shot, label);
	}
	api::render_pass& device::create_render_pass(temporary_return_t, std::span<const api::render_pass::color_attachment> colors, const optional<api::render_pass::depth_stencil_attachment>& depth /* = {} */, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Render Pass" */) {
		static webgpu::render_pass render_pass;
		return render_pass = create_render_pass(colors, depth, one_shot, label);
	}

	webgpu::compute_pipeline device::create_compute_pipeline(const pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		return webgpu::compute_pipeline::create(*this, entry_point, label);
	}
	api::compute_pipeline& device::create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		static webgpu::compute_pipeline pipeline;
		return pipeline = create_compute_pipeline(entry_point, label);
	}
	
	webgpu::render_pipeline device::create_render_pipeline(const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments /* = {} */, optional<depth_stencil_attachment> depth_attachment /* = {} */, const api::render_pipeline::config& config /*=  {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		return webgpu::render_pipeline::create(*this, entry_points, color_attachments, depth_attachment, config, label);
	}
	api::render_pipeline& device::create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments /* = {} */, const optional<depth_stencil_attachment>& depth_attachment /* = {} */, const api::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		static webgpu::render_pipeline pipeline;
		return pipeline = create_render_pipeline(entry_points, color_attachments, depth_attachment, config, label);
	}
	webgpu::render_pipeline device::create_render_pipeline_from_compatible_render_pass(const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const api::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		return webgpu::render_pipeline::create_from_compatible_render_pass(*this, entry_points, compatible_render_pass, config, label);
	}
	api::render_pipeline& device::create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const api::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		static webgpu::render_pipeline pipeline;
		return pipeline = create_render_pipeline_from_compatible_render_pass(entry_points, compatible_render_pass, config, label);
	}

	static_assert(device_concept<device>);
}