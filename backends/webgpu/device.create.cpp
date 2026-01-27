#include "../../util/error_macros.hpp"
#include <webgpu/webgpu.h>

import std.compat;
import slcross;
import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;
import stylizer.graphics;
import stylizer.errors;

namespace stylizer::graphics::webgpu {
	webgpu::device device::create_default(const webgpu::device::create_config& config /* = {} */) {
		device out;

		WGPURequestAdapterOptions options = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
		options.featureLevel = WGPUFeatureLevel_Core;
		options.powerPreference = config.high_performance ? WGPUPowerPreference_HighPerformance : WGPUPowerPreference_LowPower;
		options.compatibleSurface = config.compatible_surface ? confirm_webgpu_type<webgpu::surface>(*config.compatible_surface).surface_ : nullptr;
		wait_for_future(wgpuInstanceRequestAdapter(get_common_instance(), &options, {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata, void*){
				WGPUAdapter& out = *(WGPUAdapter*)userdata;
				switch (status) {
				case WGPURequestAdapterStatus_CallbackCancelled: [[fallthrough]];
				case WGPURequestAdapterStatus_Unavailable: [[fallthrough]];
				case WGPURequestAdapterStatus_Error:
					STYLIZER_THROW(from_webgpu(message));
					[[fallthrough]];
				case WGPURequestAdapterStatus_Success: [[fallthrough]];
				case WGPURequestAdapterStatus_Force32:
					out = adapter;
				break;
				}
			},
			.userdata1 = &out.adapter, .userdata2 = nullptr
		}));

		WGPUFeatureName float32filterable = WGPUFeatureName_Float32Filterable;
		WGPUDeviceDescriptor device = WGPU_DEVICE_DESCRIPTOR_INIT;
		device.label = to_webgpu(config.label);
		device.requiredFeatureCount = 1,
		device.requiredFeatures = &float32filterable,
		device.requiredLimits = nullptr,
		device.defaultQueue = { .label = to_webgpu(config.queue_label) },
		device.uncapturedErrorCallbackInfo = {
			.callback = [](WGPUDevice const * device, WGPUErrorType type, WGPUStringView message, void* userdata1, void* userdata2) {
				get_error_handler()(stylizer::error_severity::Error, from_webgpu(message), (size_t)type);
			}
		};
		device.deviceLostCallbackInfo = {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPUDevice const * device, WGPUDeviceLostReason reason, WGPUStringView message, void* userdata1, void* userdata2) {
				if (reason == WGPUDeviceLostReason_Destroyed) return;
				STYLIZER_THROW(from_webgpu(message));
			}
		};
		wait_for_future(wgpuAdapterRequestDevice(out.adapter, &device, {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata, void*){
				WGPUDevice& out = *(WGPUDevice*)userdata;
				switch (status) {
				case WGPURequestDeviceStatus_CallbackCancelled: [[fallthrough]];
				case WGPURequestDeviceStatus_Error:
					STYLIZER_THROW(from_webgpu(message));
					[[fallthrough]];
				case WGPURequestDeviceStatus_Success: [[fallthrough]];
				case WGPURequestDeviceStatus_Force32:
					out = device;
				}
			},
			.userdata1 = &out.device_, .userdata2 = nullptr
		}));

		out.queue = wgpuDeviceGetQueue(out.device_);
		return out;
	}

    webgpu::buffer device::create_buffer(usage usage, size_t size, bool mapped_at_creation /* = false */, const std::string_view label /* = "Stylizer Buffer" */) {
		return webgpu::buffer::create(*this, usage, size, mapped_at_creation, label);
	}

	graphics::buffer& device::create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation /* = false */, const std::string_view label /* = "Stylizer Buffer" */) {
		static webgpu::buffer temp;
		return temp = create_buffer(usage, size, mapped_at_creation);
	}

	webgpu::buffer device::create_and_write_buffer(usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		return webgpu::buffer::create_and_write(*this, usage, data, offset, label);
	}

	graphics::buffer& device::create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		static webgpu::buffer temp;
		return temp = create_and_write_buffer(usage, data, offset, label);
	}
	
	template<typename T> requires(!std::same_as<T, std::byte>)
	webgpu::buffer device::create_and_write_buffer(graphics::usage usage, std::span<const T> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		return create_and_write_buffer(usage, byte_span(data), offset, label);
	}

    webgpu::command_encoder device::create_command_encoder(bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		return webgpu::command_encoder::create(*this, one_shot, label);
	}

	graphics::command_encoder& device::create_command_encoder(temporary_return_t, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		static webgpu::command_encoder temp;
		return temp = create_command_encoder(one_shot, label);
	}

    webgpu::compute_pipeline device::create_compute_pipeline(const pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		return webgpu::compute_pipeline::create(*this, entry_point, label);
	}

	graphics::compute_pipeline& device::create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label /* = "Stylizer Compute Pipeline" */) {
		static webgpu::compute_pipeline temp;
		return temp = create_compute_pipeline(entry_point, label);
	}

	webgpu::render_pass device::create_render_pass(std::span<const graphics::render_pass::color_attachment> colors, std::optional<graphics::render_pass::depth_stencil_attachment> depth /* = {} */, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Render Pass" */) {
		return webgpu::render_pass::create(*this, colors, depth, one_shot, label);
	}

	graphics::render_pass& device::create_render_pass(temporary_return_t, std::span<const graphics::render_pass::color_attachment> colors, const std::optional<graphics::render_pass::depth_stencil_attachment>& depth /* = {} */, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Render Pass" */) {
		static webgpu::render_pass temp;
		return temp = create_render_pass(colors, depth, one_shot, label);
	}

	webgpu::render_pipeline device::create_render_pipeline(const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments /* = {} */, std::optional<depth_stencil_attachment> depth_attachment /* = {} */, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		return webgpu::render_pipeline::create(*this, entry_points, color_attachments, depth_attachment, config, label);
	}

	graphics::render_pipeline& device::create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments /* = {} */, const std::optional<depth_stencil_attachment>& depth_attachment /* = {} */, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		static webgpu::render_pipeline temp;
		return temp = create_render_pipeline(entry_points, color_attachments, depth_attachment, config, label);
	}

	webgpu::render_pipeline device::create_render_pipeline_from_compatible_render_pass(const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		return webgpu::render_pipeline::create_from_compatible_render_pass(*this, entry_points, compatible_render_pass, config, label);
	}

	graphics::render_pipeline& device::create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config /* = {} */, const std::string_view label /* = "Stylizer Render Pipeline" */) {
		static webgpu::render_pipeline temp;
		return temp = create_render_pipeline_from_compatible_render_pass(entry_points, compatible_render_pass, config, label);
	}

	webgpu::shader device::create_shader_from_wgsl(const std::string_view wgsl, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_wgsl(*this, wgsl, label);
	}
	graphics::shader& device::create_shader_from_wgsl(temporary_return_t, const std::string_view wgsl, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_wgsl(wgsl, label);
	}

	webgpu::shader device::create_shader_from_session(shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_session(*this, stage, session, entry_point, label);
	}
	graphics::shader& device::create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_session(stage, session, entry_point, label);
	}

	webgpu::shader device::create_shader_from_spirv(shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_spirv(*this, stage, spirv, entry_point, label);
	}
	graphics::shader& device::create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_spirv(stage, spirv, entry_point, label);
	}

	webgpu::shader device::create_shader_from_source(shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		return webgpu::shader::create_from_source(*this, lang, stage, source, entry_point.value_or("main"), label);
	}
	graphics::shader& device::create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point /* = "main" */, const std::string_view label /* = "Stylizer Shader" */) {
		static webgpu::shader temp;
		return temp = create_shader_from_source(lang, stage, source, entry_point.value_or("main"), label);
	}

	webgpu::texture device::create_texture(const graphics::texture::create_config& config /* = {} */) {
		return webgpu::texture::create(*this, config);
	}

	graphics::texture& device::create_texture(temporary_return_t, const graphics::texture::create_config& config /* = {} */) {
		static webgpu::texture temp;
		return temp = create_texture(config);
	}

	webgpu::texture device::create_and_write_texture(std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config /* = {} */) {
		return webgpu::texture::create_and_write(*this, data, layout, config);
	}

	graphics::texture& device::create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config /* = {} */) {
		static webgpu::texture temp;
		return temp = create_and_write_texture(data, layout, config);
	}
}