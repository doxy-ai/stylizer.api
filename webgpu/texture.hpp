#pragma once

#include "device.hpp"
#include "cstring_from_view.hpp"
#include <cstdint>
#include <webgpu/webgpu.hpp>

namespace stylizer::api::webgpu {
	struct texture: public api::texture { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(texture);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::Texture texture_ = nullptr;
		wgpu::TextureView view = nullptr;
		wgpu::Sampler sampler = nullptr;

		inline texture(texture&& o) { *this = std::move(o); }
		inline texture& operator=(texture&& o) {
			texture_ = std::exchange(o.texture_, nullptr);
			view = std::exchange(o.view, nullptr);
			sampler = std::exchange(o.sampler, nullptr);
			return *this;
		}
		inline operator bool() const override { return texture_ || view || sampler; }
		texture&& move() { return std::move(*this); }


		static texture create(api::device& device_, const create_config& config = {}) {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			WGPUTextureFormat format = to_wgpu(config.format);
			texture out;
			out.texture_ = device.device_.createTexture(WGPUTextureDescriptor{
				.label = cstring_from_view(config.label),
				.usage = to_wgpu_texture(config.usage),
				.dimension = config.size.y > 1 ? config.size.z > 1 ? wgpu::TextureDimension::_3D : wgpu::TextureDimension::_2D : wgpu::TextureDimension::_1D,
				.size = {static_cast<uint32_t>(config.size.x), static_cast<uint32_t>(config.size.y), static_cast<uint32_t>(config.size.z)},
				.format = format,
				.mipLevelCount = config.mip_levels,
				.sampleCount = config.samples,
				.viewFormatCount = 1,
				.viewFormats = &format,
			});
			out.create_view(true);
			return out;
		}

		static texture create_and_write(api::device& device, std::span<const std::byte> data, const data_layout& layout, create_config config = {}) {
			config.size = {data.size() / layout.rows_per_image / bytes_per_pixel(config.format), layout.rows_per_image, 1};
			auto out = create(device, config);
			out.write(device, data, layout, config.size);
			return out;
		}

		wgpu::TextureView create_view(const wgpu::TextureViewDescriptor& descript, bool save = true) {
			auto out = texture_.createView(descript);
			if(save) view = out;
			return out;
		}
		wgpu::TextureView create_view(bool save = true) {
			return create_view(WGPUTextureViewDescriptor{
				.format = texture_.getFormat(),
				.dimension = texture_.getDepthOrArrayLayers() > 0 ? wgpu::TextureViewDimension::_2D : wgpu::TextureViewDimension::_3D,
				.baseMipLevel = 0,
				.mipLevelCount = texture_.getMipLevelCount(),
				.baseArrayLayer = 0,
				.arrayLayerCount = std::max<uint32_t>(texture_.getDepthOrArrayLayers(), 1),
				.aspect = wgpu::TextureAspect::All,
			}, save);
		}
		wgpu::TextureView create_mip_view(uint32_t mip_level) {
			return create_view(WGPUTextureViewDescriptor{
				.format = texture_.getFormat(),
				.dimension = texture_.getDepthOrArrayLayers() > 0 ? wgpu::TextureViewDimension::_2D : wgpu::TextureViewDimension::_3D,
				.baseMipLevel = mip_level,
				.mipLevelCount = 1,
				.baseArrayLayer = 0,
				.arrayLayerCount = std::max<uint32_t>(texture_.getDepthOrArrayLayers(), 1),
				.aspect = wgpu::TextureAspect::All,
			}, false);
		}




		vec3u size() const override {
			auto& texture = const_cast<wgpu::Texture&>(texture_);
			return {texture.getWidth(), texture.getHeight(), texture.getDepthOrArrayLayers()};
		}

		enum texture_format texture_format() const override {
			return from_wgpu(const_cast<wgpu::Texture&>(texture_).getFormat());
		}

		enum usage usage() const override {
			return from_wgpu(const_cast<wgpu::Texture&>(texture_).getUsage());
		}

		uint32_t mip_levels() const override {
			return const_cast<wgpu::Texture&>(texture_).getMipLevelCount();
		}
		uint32_t samples() const override {
			return const_cast<wgpu::Texture&>(texture_).getSampleCount();
		}




		api::texture& configure_sampler(api::device& device_, const sampler_config& config = {}) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			if(sampler) sampler.release();
			sampler = device.device_.createSampler(WGPUSamplerDescriptor{
				.label = cstring_from_view(config.label),
				.addressModeU = to_wgpu(config.mode),
				.addressModeV = to_wgpu(config.mode),
				.addressModeW = to_wgpu(config.mode),
				.magFilter = config.magnify_linear ? wgpu::FilterMode::Linear : wgpu::FilterMode::Nearest,
				.minFilter = config.minify_linear ? wgpu::FilterMode::Linear : wgpu::FilterMode::Nearest,
				.mipmapFilter = config.mip_linear ? wgpu::MipmapFilterMode::Linear : wgpu::MipmapFilterMode::Nearest,
				.lodMinClamp = config.lod_min_clamp,
				.lodMaxClamp = config.lod_max_clamp,
				.compare = to_wgpu(config.depth_comparison_function),
				.maxAnisotropy = static_cast<uint16_t>(config.max_anisotropy),
			});
			return *this;
		}

		bool sampled() const override {
			return sampler;
		}

		api::texture& write(api::device& device_, std::span<const std::byte> data, const data_layout& layout, vec3u extent, vec3u origin = {0, 0, 0}, size_t mip_level = 0) override {
			assert(data.size() >= layout.offset + layout.bytes_per_row * layout.rows_per_image);
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			device.device_.getQueue().writeTexture(WGPUImageCopyTexture{
				.texture = texture_,
				.mipLevel = static_cast<uint32_t>(mip_level),
				.origin = {static_cast<uint32_t>(origin.x), static_cast<uint32_t>(origin.y), static_cast<uint32_t>(origin.z)},
				.aspect = wgpu::TextureAspect::All, // TODO: Should provide control over this?
			}, data.data(), data.size(), WGPUTextureDataLayout{
				.offset = layout.offset,
				.bytesPerRow = static_cast<uint32_t>(layout.bytes_per_row),
				.rowsPerImage = static_cast<uint32_t>(layout.rows_per_image)
			}, {static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), static_cast<uint32_t>(extent.z)});

			return *this;
		}


		static inline void copy_texture_to_texture_impl(wgpu::CommandEncoder e, webgpu::texture& destination, const webgpu::texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) {
			vec3u max_origin = {std::max(destination_origin.x, source_origin.x), std::max(destination_origin.y, source_origin.y), std::max(destination_origin.z, source_origin.z)};
			vec3u destSize = destination.size();
			vec3u srcSize = source.size();
			vec3u minSize = {std::max(destSize.x, srcSize.x), std::max(destSize.y, srcSize.y), std::max(destSize.z, srcSize.z)};
			vec3u extent = extent_override.value_or(vec3u{minSize.x - max_origin.x, minSize.y - max_origin.y, minSize.z - max_origin.z});
			size_t mip_levels = mip_levels_override.value_or(std::min(destination.mip_levels(), source.mip_levels()) - min_mip_level);

			for(uint32_t mip = min_mip_level; mip < mip_levels; ++mip)
				e.copyTextureToTexture(WGPUImageCopyTexture{
					.texture = source.texture_,
					.mipLevel = mip,
					.origin = {static_cast<uint32_t>(source_origin.x), static_cast<uint32_t>(source_origin.y), static_cast<uint32_t>(source_origin.z)},
					.aspect = wgpu::TextureAspect::All
				}, WGPUImageCopyTexture{
					.texture = destination.texture_,
					.mipLevel = mip,
					.origin = {static_cast<uint32_t>(destination_origin.x), static_cast<uint32_t>(destination_origin.y), static_cast<uint32_t>(destination_origin.z)},
					.aspect = wgpu::TextureAspect::All
				}, {static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), static_cast<uint32_t>(extent.z)});
		}

		api::texture& copy_from(api::device& device_, const api::texture& source_, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			auto& source = confirm_wgpu_type<webgpu::texture>(source_);
			auto_release e = device.device_.createCommandEncoder();
			copy_texture_to_texture_impl(e, *this, source, destination_origin, source_origin, extent_override, min_mip_level, mip_levels_override);
			auto_release commands = e.finish();
			device.queue.submit(commands);
			return *this;
		}

		api::texture& blit_from(api::device& device, const api::texture& source, std::optional<color32> clear_value = {}, api::render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) override;


		api::texture& generate_mipmaps(api::device& device, size_t first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override;


		void release() override {
			if(texture_) std::exchange(texture_, nullptr).release();
			if(view) std::exchange(view, nullptr).release();
			if(sampler) std::exchange(sampler, nullptr).release();
		}
	};
	static_assert(texture_concept<texture>);
}