#pragma once

#include "device.hpp"
#include "cstring_from_view.hpp"

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
		inline operator bool() { return texture_ || view || sampler; }

		static texture create(api::device& device_, const create_config& config = {}) {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			WGPUTextureFormat format = to_wgpu(config.format);
			texture out;
			out.texture_ = device.device_.createTexture(WGPUTextureDescriptor{
				.label = cstring_from_view(config.label),
				.usage = to_wgpu_texture(config.usage),
				.dimension = config.size.y > 0 ? config.size.z > 0 ? wgpu::TextureDimension::_3D : wgpu::TextureDimension::_2D : wgpu::TextureDimension::_1D,
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

		api::texture& write(api::device& device_, std::span<std::byte> data, data_layout layout, vec3u extent, vec3u origin = {0, 0, 0}, size_t mip_level = 0) override {
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

		void release() override {
			if(texture_) std::exchange(texture_, nullptr).release();
			if(view) std::exchange(view, nullptr).release();
			if(sampler) std::exchange(sampler, nullptr).release();
		}
	};
	static_assert(texture_concept<texture>);
}