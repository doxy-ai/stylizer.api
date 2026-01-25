module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:command_encoder;

import :command_encoder_base;

namespace stylizer::graphics::webgpu {

	export struct command_encoder : public command_encoder_base<graphics::command_encoder, command_encoder> { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_encoder);
		using super = webgpu::command_encoder_base<graphics::command_encoder, command_encoder>;
		inline command_encoder(command_encoder&& o) { *this = std::move(o); }
		inline command_encoder& operator=(command_encoder&& o) { super::operator=(std::move(o)); return *this; }

		stylizer::auto_release<command_encoder> auto_release() { return std::move(*this); }
	};
	static_assert(command_encoder_concept<command_encoder>);

	webgpu::command_encoder device::create_command_encoder(bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		return webgpu::command_encoder::create(*this, one_shot, label);
	}

	graphics::command_encoder& device::create_command_encoder(temporary_return_t, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) {
		static webgpu::command_encoder temp;
		return temp = create_command_encoder(one_shot, label);
	}

	graphics::texture& webgpu::texture::generate_mipmaps(graphics::device& device_, std::optional<size_t> first_mip_level_ /* = 0 */, std::optional<size_t> mip_levels_override /* = {} */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		auto size = this->size();
		size_t size_max_levels = std::bit_width(std::min(size.x, size.y));
		size_t first_mip_level = first_mip_level_.value_or(0);
		uint32_t mip_levels = first_mip_level + mip_levels_override.value_or(size_max_levels - first_mip_level);
		assert(mip_levels <= size_max_levels);

		auto formatStr = format_to_string(wgpuTextureGetFormat(texture_));
		stylizer::auto_release mipShader = webgpu::shader::create_from_wgsl(device, R"_(
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

		stylizer::auto_release new_texture = device.create_texture({.format = this->texture_format(), .usage = usage() | usage::CopyDestination | usage::Storage, .size = size, .mip_levels = mip_levels, .samples = samples()});
		new_texture.copy_from(device, *this, {}, {}, {}, 0, 1);

		stylizer::auto_release pipeline = device.create_compute_pipeline({&mipShader, "compute"}, "Stylizer Mipmapping Pipeline");
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