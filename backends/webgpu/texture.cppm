module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"
#include "../../util/defer.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:texture;

import :device;
import :texture_view;

namespace stylizer::graphics::webgpu {

	export struct texture : public graphics::texture { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(texture); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture);
		uint32_t type = magic_number;
		WGPUTexture texture_ = nullptr;
		WGPUSampler sampler = nullptr;
		mutable texture_view view = {}; // Mutable so may update while texture is const!

		inline texture(texture&& o) { *this = std::move(o); }
		inline texture& operator=(texture&& o) {
			texture_ = std::exchange(o.texture_, nullptr);
			sampler = std::exchange(o.sampler, nullptr);
			view.release(); // Pointers have been invalidated
			return *this;
		}
		inline operator bool() const override { return texture_ || sampler; }

		static texture create(graphics::device& device_, const create_config& config = {});

		static texture create_and_write(graphics::device& device, std::span<const std::byte> data, const data_layout& layout, create_config config = {});

		texture_view create_view(graphics::device& device, const view::create_config& config = {}) const;
		graphics::texture_view& create_view(temporary_return_t, graphics::device& device, const view::create_config& config = {}) const override;

		const graphics::texture_view& full_view(graphics::device& device, bool treat_as_cubemap = false) const override;

		vec3u size() const override;

		format texture_format() const override;

		graphics::usage usage() const override;

		uint32_t mip_levels() const override;
		uint32_t samples() const override;

		graphics::texture& configure_sampler(graphics::device& device_, const sampler_config& config = {}) override;

		bool sampled() const override;

		graphics::texture& write(graphics::device& device_, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin_ = {{ 0, 0, 0 }}, size_t mip_level = 0) override;

		graphics::texture& copy_from(graphics::device& device_, const graphics::texture& source_, std::optional<vec3u> destination_origin = {{0, 0, 0}}, std::optional<vec3u> source_origin = {{0, 0, 0}}, std::optional<vec3u> extent_overrideoverride = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override;

		graphics::texture& blit_from(graphics::device& device_, const graphics::texture& source_, std::optional<color32> clear_value = {}, graphics::render_pipeline* pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) override;

		graphics::texture& generate_mipmaps(graphics::device& device_, std::optional<size_t> first_mip_level_ = 0, std::optional<size_t> mip_levels_override = {}) override;

		void release() override;
		stylizer::auto_release<texture> auto_release() { return std::move(*this); }
	};
	static_assert(texture_concept<texture>);

	export void copy_texture_to_texture_impl(WGPUCommandEncoder e, webgpu::texture& destination, const webgpu::texture& source, vec3u destination_origin /* = {} */, vec3u source_origin /* = {} */, std::optional<vec3u> extent_override /* = {} */, size_t min_mip_level /* = 0 */, std::optional<size_t> mip_levels_override /* = {} */) {
		vec3u max_origin = { std::max(destination_origin.x, source_origin.x), std::max(destination_origin.y, source_origin.y), std::max(destination_origin.z, source_origin.z) };
		vec3u destSize = destination.size();
		vec3u srcSize = source.size();
		vec3u minSize = { std::max(destSize.x, srcSize.x), std::max(destSize.y, srcSize.y), std::max(destSize.z, srcSize.z) };
		vec3u extent = extent_override.value_or(vec3u { minSize.x - max_origin.x, minSize.y - max_origin.y, minSize.z - max_origin.z });
		size_t mip_levels = mip_levels_override.value_or(std::min(destination.mip_levels(), source.mip_levels()) - min_mip_level);

		for (uint32_t mip = min_mip_level; mip < mip_levels; ++mip){
			WGPUTexelCopyTextureInfo src = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
			src.texture = source.texture_;
			src.mipLevel = mip;
			src.origin = { static_cast<uint32_t>(source_origin.x), static_cast<uint32_t>(source_origin.y), static_cast<uint32_t>(source_origin.z) };
			src.aspect = WGPUTextureAspect_All;
			WGPUTexelCopyTextureInfo dest = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
			dest.texture = destination.texture_;
			dest.mipLevel = mip;
			dest.origin = { static_cast<uint32_t>(destination_origin.x), static_cast<uint32_t>(destination_origin.y), static_cast<uint32_t>(destination_origin.z) };
			dest.aspect = WGPUTextureAspect_All;
			WGPUExtent3D size = { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), static_cast<uint32_t>(extent.z) };
			wgpuCommandEncoderCopyTextureToTexture(e, &src, &dest, &size);
		}
	}
}