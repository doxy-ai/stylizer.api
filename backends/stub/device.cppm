module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:device;

import std.compat;
import slcross;
import stylizer.graphics;
import stylizer.graphics.string2magic;
import stylizer.auto_release;
import stylizer.errors;

namespace stylizer::graphics::stub {
	export constexpr uint32_t magic_number = string2magic("STUB");

	export struct texture;
	export struct buffer;
	export struct shader;
	export struct command_encoder;
	export struct render_pass;
	export struct compute_pipeline;
	export struct render_pipeline;

	export namespace compute {
		using pipeline = compute_pipeline;
		using pass = command_encoder;
	}

	export namespace render {
		using pipeline = render_pipeline;
		using pass = render_pass;
	}

	export struct device : public graphics::device { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(device); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(device);
		uint32_t type = magic_number;

		inline device(device&& o) { *this = std::move(o); }
		inline device& operator=(device&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static stub::device create_default(const stub::device::create_config& config = {}) { STYLIZER_THROW("Not implemented yet!"); }

		bool per_frame(bool wait_for_queues = true) override { STYLIZER_THROW("Not implemented yet!"); }

		stub::texture create_texture(const graphics::texture::create_config& config = {});
		graphics::texture& create_texture(graphics::temporary_return_t, const graphics::texture::create_config& config = {}) override;
		stub::texture create_and_write_texture(std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config = {});
		graphics::texture& create_and_write_texture(graphics::temporary_return_t, std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config = {}) override;

		stub::buffer create_buffer(graphics::usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer");
		graphics::buffer& create_buffer(graphics::temporary_return_t, graphics::usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") override;
		stub::buffer create_and_write_buffer(graphics::usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer");
		graphics::buffer& create_and_write_buffer(graphics::temporary_return_t, graphics::usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") override;
		template<typename T> requires(!std::same_as<T, std::byte>)
		stub::buffer create_and_write_buffer(graphics::usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer");
		template<typename T> requires(!std::same_as<T, std::byte>)
		graphics::buffer& create_and_write_buffer(graphics::temporary_return_t, graphics::usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write_buffer(usage, byte_span(data), offset, label);
		}

		stub::shader create_shader_from_session(graphics::shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");
		graphics::shader& create_shader_from_session(graphics::temporary_return_t, graphics::shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") override;
		stub::shader create_shader_from_spirv(graphics::shader::stage stage, graphics::spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");
		graphics::shader& create_shader_from_spirv(graphics::temporary_return_t, graphics::shader::stage stage, graphics::spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") override;
		stub::shader create_shader_from_source(graphics::shader::language lang, graphics::shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader");
		graphics::shader& create_shader_from_source(graphics::temporary_return_t, graphics::shader::language lang, graphics::shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") override;

		stub::command_encoder create_command_encoder(bool one_shot = false, const std::string_view label = "Stylizer Command Encoder");
		graphics::command_encoder& create_command_encoder(graphics::temporary_return_t, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") override;

		stub::render_pass create_render_pass(std::span<const graphics::render_pass::color_attachment> colors, std::optional<graphics::render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass");
		graphics::render_pass& create_render_pass(graphics::temporary_return_t, std::span<const graphics::render_pass::color_attachment> colors, const std::optional<graphics::render_pass::depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") override;

		stub::compute_pipeline create_compute_pipeline(const graphics::pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline");
		graphics::compute_pipeline& create_compute_pipeline(graphics::temporary_return_t, const graphics::pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") override;

		stub::render_pipeline create_render_pipeline(const graphics::pipeline::entry_points& entry_points, std::span<const graphics::color_attachment> color_attachments = {}, std::optional<graphics::depth_stencil_attachment> depth_attachment = {}, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline");
		graphics::render_pipeline& create_render_pipeline(graphics::temporary_return_t, const graphics::pipeline::entry_points& entry_points, std::span<const graphics::color_attachment> color_attachments = {}, const std::optional<graphics::depth_stencil_attachment>& depth_attachment = {}, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override;
		stub::render_pipeline create_render_pipeline_from_compatible_render_pass(const graphics::pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline");
		graphics::render_pipeline& create_render_pipeline_from_compatible_render_pass(graphics::temporary_return_t, const graphics::pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override;

		void release(bool static_sub_objects = false) override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<device> auto_release() { return std::move(*this); }
	};
	// static_assert(device_concept<device>);
} // namespace stylizer::graphics::stub