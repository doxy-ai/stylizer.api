#include "texture_view.hpp"
#include "texture.hpp"

namespace stylizer::api::webgpu {
	texture_view create_view(const wgpu::TextureViewDescriptor& descript, const webgpu::texture& texture) {
		texture_view out;
		out.owning_texture = &texture;
		out.view = const_cast<wgpu::Texture&>(texture.texture_).createView(descript);
		return out;
	}
	
	texture_view texture_view::create(api::device& device, const api::texture& texture_, const texture_view::create_config& config /* = {} */) {
		// auto& device = confirm_wgpu_type<webgpu::device>(device_);
		auto& texture = confirm_wgpu_type<webgpu::texture>(texture_);

        return create_view(WGPUTextureViewDescriptor{
			.format = to_wgpu(texture.texture_format()),
			.dimension = texture.size().z > 1 ? wgpu::TextureViewDimension::_3D : wgpu::TextureViewDimension::_2D,
			.baseMipLevel = static_cast<uint32_t>(config.base_mip_level),
			.mipLevelCount = static_cast<uint32_t>(config.mip_level_count_override.value_or(texture.mip_levels())),
			.baseArrayLayer = 0,
			.arrayLayerCount = const_cast<wgpu::Texture&>(texture.texture_).getDepthOrArrayLayers(),
			.aspect = to_wgpu(config.aspect),
		}, texture);
	}

	const api::texture& texture_view::texture() const { return *owning_texture; }
}