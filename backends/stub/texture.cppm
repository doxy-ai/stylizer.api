module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:texture;

import :device;
import :texture_view;

namespace stylizer::graphics::stub {

	export struct texture : public graphics::texture { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(texture); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture);
		uint32_t type = magic_number;

		inline texture(texture&& o) { *this = std::move(o); }
		inline texture& operator=(texture&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static stub::texture create(graphics::device& device, const create_config& config = {}) { STYLIZER_THROW("Not implemented yet!"); }
		static stub::texture create_and_write(graphics::device& device, std::span<const std::byte> data, const data_layout& layout, create_config config = {}) { STYLIZER_THROW("Not implemented yet!"); }

		stub::texture_view create_view(graphics::device& device, const view::create_config& config = {}) const { STYLIZER_THROW("Not implemented yet!"); }
		graphics::texture_view& create_view(temporary_return_t, graphics::device& device, const view::create_config& config = {}) const override { STYLIZER_THROW("Not implemented yet!"); }
		const graphics::texture_view& full_view(graphics::device& device, bool treat_as_cubemap = false) const override { STYLIZER_THROW("Not implemented yet!"); }

		vec3u size() const override { STYLIZER_THROW("Not implemented yet!"); }
		enum texture_format texture_format() const override { STYLIZER_THROW("Not implemented yet!"); }
		enum usage usage() const override { STYLIZER_THROW("Not implemented yet!"); }
		uint32_t mip_levels() const override { STYLIZER_THROW("Not implemented yet!"); }
		uint32_t samples() const override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::texture& configure_sampler(graphics::device& device, const sampler_config& config = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		bool sampled() const override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::texture& write(graphics::device& device, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin = { { 0, 0, 0 } }, size_t mip_level = 0) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::texture& copy_from(graphics::device& device, const graphics::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		graphics::texture& blit_from(graphics::device& device, const graphics::texture& source, std::optional<color32> clear_value = {}, STYLIZER_NULLABLE graphics::render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::texture& generate_mipmaps(graphics::device& device, std::optional<size_t> first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<texture> auto_release() { return std::move(*this); }
	};
	static_assert(texture_concept<texture>);

	texture device::create_texture(const graphics::texture::create_config& config /* = {} */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::texture& device::create_texture(temporary_return_t, const graphics::texture::create_config& config /* = {} */) { STYLIZER_THROW("Not implemented yet!"); }
	texture device::create_and_write_texture(std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config /* = {} */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::texture& device::create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config /* = {} */) { STYLIZER_THROW("Not implemented yet!"); }
}