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

	inline enum texture_view::create_config::aspect from_webgpu(WGPUTextureAspect aspect){
		switch(aspect){
		case WGPUTextureAspect_All: return texture_view::create_config::aspect::All;
		case WGPUTextureAspect_DepthOnly: return texture_view::create_config::aspect::DepthOnly;
		case WGPUTextureAspect_StencilOnly: return texture_view::create_config::aspect::StencilOnly;
		default:
			STYLIZER_THROW(std::string("Failed to find texture aspect: ") + std::string(magic_enum::enum_name(aspect)));
		}
		std::unreachable();
	}

	inline WGPUTextureAspect to_webgpu(enum texture_view::create_config::aspect aspect){
		switch(aspect){
		case texture_view::create_config::aspect::All: return WGPUTextureAspect_All;
		case texture_view::create_config::aspect::DepthOnly: return WGPUTextureAspect_DepthOnly;
		case texture_view::create_config::aspect::StencilOnly: return WGPUTextureAspect_StencilOnly;
		}
		std::unreachable();
	}

	inline graphics::texture::address_mode from_webgpu(WGPUAddressMode mode) {
		switch (mode) {
		case WGPUAddressMode_Repeat: return graphics::texture::address_mode::Repeat;
		case WGPUAddressMode_MirrorRepeat: return graphics::texture::address_mode::MirrorRepeat;
		case WGPUAddressMode_ClampToEdge: return graphics::texture::address_mode::ClampToEdge;
		default:
			STYLIZER_THROW(std::string("Failed to find address mode: ") + std::string(magic_enum::enum_name(mode)));
		}
		std::unreachable();
	}

	inline WGPUAddressMode to_webgpu(graphics::texture::address_mode mode) {
		switch (mode) {
		case graphics::texture::address_mode::Repeat: return WGPUAddressMode_Repeat;
		case graphics::texture::address_mode::MirrorRepeat: return WGPUAddressMode_MirrorRepeat;
		case graphics::texture::address_mode::ClampToEdge: return WGPUAddressMode_ClampToEdge;
		}
		std::unreachable();
	}

	texture_view create_view(const WGPUTextureViewDescriptor& descript, const webgpu::texture& texture);
	void copy_texture_to_texture_impl(WGPUCommandEncoder e, webgpu::texture& destination, const webgpu::texture& source, vec3u destination_origin = {}, vec3u source_origin = {}, std::optional<vec3u> extent_override = {}, size_t min_mip_level = 0, std::optional<size_t> mip_levels_override = {});

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

		static texture create(graphics::device& device_, const create_config& config /* = {} */) {
			auto& device = confirm_webgpu_type<webgpu::device>(device_);

			WGPUTextureFormat format = to_webgpu(config.format);
			texture out;
			WGPUTextureDescriptor d = WGPU_TEXTURE_DESCRIPTOR_INIT;
			d.label = to_webgpu(config.label),
			d.usage = to_webgpu_texture(config.usage),
			d.dimension = config.size.y > 1 ? config.size.z > 1 ? WGPUTextureDimension_3D : WGPUTextureDimension_2D : WGPUTextureDimension_1D,
			d.size = { static_cast<uint32_t>(config.size.x), static_cast<uint32_t>(config.size.y), static_cast<uint32_t>(config.size.z) },
			d.format = format,
			d.mipLevelCount = config.mip_levels,
			d.sampleCount = config.samples,
			d.viewFormatCount = 1,
			d.viewFormats = &format,
			out.texture_ = wgpuDeviceCreateTexture(device.device_, &d);
			return out;
		}

		static texture create_and_write(graphics::device& device, std::span<const std::byte> data, const data_layout& layout, create_config config /* = {} */) {
			using namespace magic_enum::bitwise_operators;

			config.size = { data.size() / layout.rows_per_image / bytes_per_pixel(config.format), layout.rows_per_image, 1 };
			config.usage |= graphics::usage::CopyDestination;
			auto out = create(device, config);
			out.write(device, data, layout, config.size);
			return out;
		}

		texture_view create_view(graphics::device& device, const view::create_config& config /* = {} */) const {
			return texture_view::create(device, *this, config);
		}
		graphics::texture_view& create_view(temporary_return_t, graphics::device& device, const view::create_config& config /* = {} */) const {
			static webgpu::texture_view view;
			return view = create_view(device, config);
		}

		const graphics::texture_view& full_view(graphics::device& device, bool treat_as_cubemap /* = false */) const {
			if (view) return view;
			return view = create_view(device, {.treat_as_cubemap = treat_as_cubemap}); // TODO: Do we want to expose control over the aspect?
		}

		vec3u size() const {
			auto& texture = texture_;
			return { wgpuTextureGetWidth(texture), wgpuTextureGetHeight(texture), wgpuTextureGetDepthOrArrayLayers(texture) };
		}

		format texture_format() const {
			return from_webgpu(wgpuTextureGetFormat(texture_));
		}

		graphics::usage usage() const {
			return from_webgpu_texture(wgpuTextureGetUsage(texture_));
		}

		uint32_t mip_levels() const {
			return wgpuTextureGetMipLevelCount(texture_);
		}
		uint32_t samples() const {
			return wgpuTextureGetSampleCount(texture_);
		}

		graphics::texture& configure_sampler(graphics::device& device_, const sampler_config& config = {}) {
			auto& device = confirm_webgpu_type<webgpu::device>(device_);
			if (sampler) wgpuSamplerRelease(sampler);
			WGPUSamplerDescriptor d = WGPU_SAMPLER_DESCRIPTOR_INIT;
			d.label = to_webgpu(config.label),
			d.addressModeU = to_webgpu(config.mode_x_override.value_or(config.mode)),
			d.addressModeV = to_webgpu(config.mode_y_override.value_or(config.mode)),
			d.addressModeW = to_webgpu(config.mode_z_override.value_or(config.mode)),
			d.magFilter = config.magnify_linear ? WGPUFilterMode_Linear : WGPUFilterMode_Nearest,
			d.minFilter = config.minify_linear ? WGPUFilterMode_Linear : WGPUFilterMode_Nearest,
			d.mipmapFilter = config.mip_linear ? WGPUMipmapFilterMode_Linear : WGPUMipmapFilterMode_Nearest,
			d.lodMinClamp = config.lod_min_clamp,
			d.lodMaxClamp = config.lod_max_clamp,
			d.compare = to_webgpu(config.depth_comparison_function),
			d.maxAnisotropy = static_cast<uint16_t>(config.max_anisotropy),
			sampler = wgpuDeviceCreateSampler(device.device_, &d);
			return *this;
		}

		bool sampled() const {
			return sampler;
		}

		graphics::texture& write(graphics::device& device_, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin_ = {{ 0, 0, 0 }}, size_t mip_level = 0) {
			assert(data.size() >= layout.offset + layout.bytes_per_row * layout.rows_per_image);
			auto& device = confirm_webgpu_type<webgpu::device>(device_);
			auto origin = origin_.value_or(vec3u{0, 0, 0});

			WGPUTexelCopyTextureInfo dest = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
			dest.texture = texture_;
			dest.mipLevel = mip_level;
			dest.origin = { static_cast<uint32_t>(origin.x), static_cast<uint32_t>(origin.y), static_cast<uint32_t>(origin.z) };
			dest.aspect = WGPUTextureAspect_All; // TODO: Should provide control over this?
			WGPUTexelCopyBufferLayout l = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
			l.offset = layout.offset;
			l.bytesPerRow = layout.bytes_per_row;
			l.rowsPerImage = layout.rows_per_image;
			WGPUExtent3D size = { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), static_cast<uint32_t>(extent.z) };
			wgpuQueueWriteTexture(device.queue, &dest, data.data(), data.size(), &l, &size);
			return *this;
		}

		graphics::texture& copy_from(graphics::device& device_, const graphics::texture& source_, std::optional<vec3u> destination_origin = {{0, 0, 0}}, std::optional<vec3u> source_origin = {{0, 0, 0}}, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) {
			auto& device = confirm_webgpu_type<webgpu::device>(device_);
			auto& source = confirm_webgpu_type<webgpu::texture>(source_);
			auto e = create_command_encoder(device.device_);
			defer_ { wgpuCommandEncoderRelease(e); };
			auto min_mip = min_mip_level.value_or(0);
			copy_texture_to_texture_impl(e, *this, source, destination_origin.value_or(vec3u{0, 0, 0}), source_origin.value_or(vec3u{0, 0, 0}), extent_override.value_or(vec3u{0, 0, 0}), min_mip, mip_levels_override.value_or(source.mip_levels() - min_mip));
			finish_and_submit(e, device.queue);
			return *this;
		}

		graphics::texture& blit_from(graphics::device& device_, const graphics::texture& source_, std::optional<color32> clear_value = {}, graphics::render_pipeline* pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) {
	// 		auto& device = confirm_webgpu_type<webgpu::device>(device_);
	// 		auto& source = confirm_webgpu_type<webgpu::texture>(source_);
	// 		assert(source.sampler);

	// 		auto attachment = std::array<color_attachment, 1>{color_attachment{
	// 			.texture = this,
	// 			.clear_value = clear_value
	// 		}};
	// 		auto format = texture_format();
	// 		static thread_local std::unordered_map<texture::format, stylizer::auto_release<webgpu::render_pipeline>> pipeline_cache = {};
	// 		if(!pipeline_override && pipeline_cache[format])
	// 			pipeline_override = &pipeline_cache[format];

	// 		webgpu::render_pipeline* pipeline;
	// 		if(!pipeline_override) {
	// 			static thread_local stylizer::auto_release<webgpu::shader> shader = webgpu::shader::create_from_wgsl(device, R"_(
	// @group(0) @binding(0) var texture: texture_2d<f32>;
	// @group(0) @binding(1) var sampler_: sampler;

	// struct vertex_output {
	// 	@builtin(position) position: vec4f,
	// 	@location(0) uv: vec2f
	// }

	// @vertex
	// fn vertex(@builtin(vertex_index) index: u32) -> vertex_output {
	// 	var out: vertex_output;
	// 	switch(index) {
	// 		case 0: {
	// 			out.position = vec4f(-1, -1, 0, 1);
	// 			out.uv = vec2f(0, 0);
	// 		}
	// 		case 1: {
	// 			out.position = vec4f(3, -1, 0, 1);
	// 			out.uv = vec2f(2, 0);
	// 		}
	// 		default: {
	// 			out.position = vec4f(-1, 3, 0, 1);
	// 			out.uv = vec2f(0, 2);
	// 		}
	// 	}
	// 	return out;
	// }

	// @fragment
	// fn fragment(v: vertex_output) -> @location(0) vec4f {
	// 	// let c = textureSample(texture, sampler_, vert.uv);
	// 	// return vec4f(vert.uv, 0, 1) + c * .01;
	// 	var vert = v;
	// 	vert.uv.y = -vert.uv.y; // Unflip v axis
	// 	return textureSample(texture, sampler_, vert.uv);
	// })_");
	// 			pipeline = &(pipeline_cache[format] = std::move(device.create_render_pipeline({
	// 				{shader::stage::Vertex, {&shader, "vertex"}},
	// 				{shader::stage::Fragment, {&shader, "fragment"}},
	// 			}, attachment, {}, {}, "Stylizer Blit Pipeline")));
	// 		} else pipeline = &confirm_webgpu_type<webgpu::render_pipeline>(*pipeline_override);

	// 		device.create_render_pass(attachment, {}, true, "Stylizer Blit Renderpass")
	// 			.bind_render_pipeline(device, *pipeline)
	// 			.bind_render_group(device, pipeline->create_bind_group(device, 0, std::array<bind_group::binding, 1>{
	// 				bind_group::texture_binding{&source}
	// 			}), true)
	// 			.draw(device, vertex_count_override.value_or(3))
	// 			.one_shot_submit(device);
			return *this;
		}

		graphics::texture& generate_mipmaps(graphics::device& device_, std::optional<size_t> first_mip_level_ = 0, std::optional<size_t> mip_levels_override = {}) {
	// 		auto& device = confirm_webgpu_type<webgpu::device>(device_);
	// 		auto size = this->size();
	// 		size_t size_max_levels = std::bit_width(std::min(size.x, size.y));
	// 		size_t first_mip_level = first_mip_level_.value_or(0);
	// 		uint32_t mip_levels = first_mip_level + mip_levels_override.value_or(size_max_levels - first_mip_level);
	// 		assert(mip_levels <= size_max_levels);

	// 		auto formatStr = format_to_string(wgpuTextureGetFormat(texture_));
	// 		stylizer::auto_release mipShader = webgpu::shader::create_from_wgsl(device, R"_(
	// @group(0) @binding(0) var previousMipLevel: texture_2d<f32>;
	// @group(0) @binding(1) var nextMipLevel: texture_storage_2d<)_" + formatStr + R"_(, write>;

	// @compute @workgroup_size(8, 8)
	// fn compute(@builtin(global_invocation_id) id: vec3<u32>) {
	// 	let offset = vec2<u32>(0, 1);
	// 	let color = (
	// 		textureLoad(previousMipLevel, 2 * id.xy + offset.xx, 0) +
	// 		textureLoad(previousMipLevel, 2 * id.xy + offset.xy, 0) +
	// 		textureLoad(previousMipLevel, 2 * id.xy + offset.yx, 0) +
	// 		textureLoad(previousMipLevel, 2 * id.xy + offset.yy, 0)
	// 	) * 0.25;
	// 	textureStore(nextMipLevel, id.xy, color);
	// })_");

	// 		stylizer::auto_release new_texture = device.create_texture({.format = this->texture_format(), .usage = usage() | usage::CopyDestination | usage::Storage, .size = size, .mip_levels = mip_levels, .samples = samples()});
	// 		new_texture.copy_from(device, *this, {}, {}, {}, 0, 1);

	// 		stylizer::auto_release pipeline = device.create_compute_pipeline({&mipShader, "compute"}, "Stylizer Mipmapping Pipeline");
	// 		auto encoder = device.create_command_encoder(true);
	// 		encoder.bind_compute_pipeline(device, pipeline);

	// 		// wgpu::Extent3D mipLevelSize = {size.x, size.y, 1}; // TODO: do we need a tweak to properly handle cubemaps?
	// 		for (uint32_t level = 1; level < mip_levels; ++level) {
	// 			vec3u invocationCount = {size.x / 2, size.y / 2, 1};
	// 			constexpr uint32_t workgroupSizePerDim = 8;

	// 			auto previous = new_texture.create_view(device, {.base_mip_level = level - 1, .mip_level_count_override = 1});
	// 			auto current = new_texture.create_view(device, {.base_mip_level = level, .mip_level_count_override = 1});

	// 			vec3u workgroups = {(invocationCount.x + workgroupSizePerDim + 1) / workgroupSizePerDim, (invocationCount.y + workgroupSizePerDim + 1) / workgroupSizePerDim, 1};
	// 			encoder.bind_compute_group(device, pipeline.create_bind_group(device, 0, std::array<bind_group::binding, 2>{
	// 					bind_group::texture_binding{.texture_view = &previous, .sampled_override = false}, bind_group::texture_binding{.texture_view = &current, .sampled_override = false}
	// 				}), true)
	// 				.dispatch_workgroups(device, workgroups);

	// 			size = invocationCount;
	// 			encoder.defer([view = std::move(previous)]() mutable { view.release(); });
	// 			encoder.defer([view = std::move(current)]() mutable { view.release(); });
	// 		}
	// 		encoder.one_shot_submit(device);

	// 		new_texture.sampler = std::exchange(sampler, nullptr);

	// 		this->release();
	// 		return (*this) = std::move(new_texture);
			return *this;
		}

		void release() {
			if (texture_) wgpuTextureRelease(std::exchange(texture_, nullptr));
			if (sampler) wgpuSamplerRelease(std::exchange(sampler, nullptr));
			if (view) std::exchange(view, {}).release();
		}
		stylizer::auto_release<texture> auto_release() { return std::move(*this); }
	};
	static_assert(texture_concept<texture>);

	texture_view create_view(const WGPUTextureViewDescriptor& descript, const webgpu::texture& texture) {
		texture_view out;
		out.owning_texture = &texture;
		out.view = wgpuTextureCreateView(texture.texture_, &descript);
		return out;
	}

	void copy_texture_to_texture_impl(WGPUCommandEncoder e, webgpu::texture& destination, const webgpu::texture& source, vec3u destination_origin /* = {} */, vec3u source_origin /* = {} */, std::optional<vec3u> extent_override /* = {} */, size_t min_mip_level /* = 0 */, std::optional<size_t> mip_levels_override /* = {} */) {
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

	texture_view texture_view::create(graphics::device& device, const graphics::texture& texture_, const texture_view::create_config& config /* = {} */) {
		auto& texture = confirm_webgpu_type<webgpu::texture>(texture_);
		auto size = texture.size();

		WGPUTextureViewDimension dim = WGPUTextureViewDimension_1D;
		if(config.treat_as_cubemap && size.z > 1) dim = WGPUTextureViewDimension_CubeArray;
		else if(config.treat_as_cubemap) dim = WGPUTextureViewDimension_Cube;
		else if(size.z > 1) dim = WGPUTextureViewDimension_3D;
		else if(size.y > 1) dim = WGPUTextureViewDimension_2D;

		return create_view(WGPUTextureViewDescriptor {
			.format = to_webgpu(texture.texture_format()),
			.dimension = dim,
			.baseMipLevel = static_cast<uint32_t>(config.base_mip_level),
			.mipLevelCount = static_cast<uint32_t>(config.mip_level_count_override.value_or(texture.mip_levels())),
			.baseArrayLayer = 0,
			.arrayLayerCount = wgpuTextureGetDepthOrArrayLayers(texture.texture_),
			.aspect = to_webgpu(config.aspect),
		}, texture);
	}

	const graphics::texture& texture_view::texture() const { return *owning_texture; }
}