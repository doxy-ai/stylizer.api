module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub;

import std.compat;
import slcross;
import stylizer.graphics;
import stylizer.graphics.string2magic;
import stylizer.auto_release;
import stylizer.errors;

namespace stylizer::graphics::stub {
	export constexpr uint32_t magic_number = string2magic("STUB");
	export struct device;
	export struct texture;

	export struct texture_view : public graphics::texture_view { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(texture_view); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture_view);
		uint32_t type = magic_number;

		inline texture_view(texture_view&& o) { *this = std::move(o); }
		inline texture_view& operator=(texture_view&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::texture_view create(graphics::device& device, const graphics::texture& texture, const texture_view::create_config& config = {}) { STYLIZER_API_THROW("Not implemented yet!"); }

		const graphics::texture& texture() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<texture_view> auto_release() { return std::move(*this); }
	};
	static_assert(texture_view_concept<texture_view>);

	export struct texture : public graphics::texture { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(texture); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture);
		uint32_t type = magic_number;

		inline texture(texture&& o) { *this = std::move(o); }
		inline texture& operator=(texture&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::texture create(graphics::device& device, const create_config& config = {}) { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::texture create_and_write(graphics::device& device, std::span<const std::byte> data, const data_layout& layout, create_config config = {}) { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::texture_view create_view(graphics::device& device, const view::create_config& config = {}) const { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::texture_view& create_view(temporary_return_t, graphics::device& device, const view::create_config& config = {}) const override { STYLIZER_API_THROW("Not implemented yet!"); }
		const graphics::texture_view& full_view(graphics::device& device, bool treat_as_cubemap = false) const override { STYLIZER_API_THROW("Not implemented yet!"); }

		vec3u size() const override { STYLIZER_API_THROW("Not implemented yet!"); }
		enum texture_format texture_format() const override { STYLIZER_API_THROW("Not implemented yet!"); }
		enum usage usage() const override { STYLIZER_API_THROW("Not implemented yet!"); }
		uint32_t mip_levels() const override { STYLIZER_API_THROW("Not implemented yet!"); }
		uint32_t samples() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::texture& configure_sampler(graphics::device& device, const sampler_config& config = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		bool sampled() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::texture& write(graphics::device& device, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin = { { 0, 0, 0 } }, size_t mip_level = 0) override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::texture& copy_from(graphics::device& device, const graphics::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::texture& blit_from(graphics::device& device, const graphics::texture& source, std::optional<color32> clear_value = {}, STYLIZER_NULLABLE graphics::render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::texture& generate_mipmaps(graphics::device& device, std::optional<size_t> first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<texture> auto_release() { return std::move(*this); }
	};
	static_assert(texture_concept<texture>);

	export struct buffer : public graphics::buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(buffer); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(buffer);
		uint32_t type = magic_number;

		buffer(buffer&& o) { *this = std::move(o); }
		buffer& operator=(buffer&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::buffer create(graphics::device& device, enum usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::buffer create_and_write(graphics::device& device, enum usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") { STYLIZER_API_THROW("Not implemented yet!"); }
		template<typename T>
		requires(!std::same_as<T, std::byte>)
		static stub::buffer create_and_write(graphics::device& device, enum usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write(device, usage, byte_span(data), offset, label);
		}

		static const stub::buffer& zero_buffer(graphics::device& device, enum usage usage = usage::Storage, size_t minimum_size = 0, graphics::buffer* just_released = nullptr) { STYLIZER_API_THROW("Not implemented yet!"); }
		const graphics::buffer& get_zero_buffer_singleton(graphics::device& device, enum usage usage = usage::Storage, size_t size = 0, graphics::buffer* just_released = nullptr) override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::buffer& write(graphics::device& device, std::span<const std::byte> data, size_t offset = 0) override { STYLIZER_API_THROW("Not implemented yet!"); }
		template<typename T>
		requires(!std::same_as<T, std::byte>)
		graphics::buffer& write(graphics::device& device, std::span<const T> data, size_t offset = 0) {
			return write(device, byte_span(data), offset);
		}

		size_t size() const override { STYLIZER_API_THROW("Not implemented yet!"); }
		enum usage usage() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::buffer& copy_from(graphics::device& device, const graphics::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		bool is_mapped() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		std::future<std::byte*> map_async(graphics::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		std::byte* map(graphics::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		std::byte* get_mapped_range(bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) { STYLIZER_API_THROW("Not implemented yet!"); }

		void unmap() override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<buffer> auto_release() { return std::move(*this); }
	};
	static_assert(buffer_concept<buffer>);

	export struct shader : public graphics::shader { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(shader); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(shader);
		uint32_t type = magic_number;

		shader(shader&& o) { *this = std::move(o); }
		shader& operator=(shader&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::shader create_from_wgsl(graphics::device& device, const std::string_view wgsl, const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::shader create_from_session(graphics::device& device, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::shader create_from_spirv(graphics::device& device, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::shader create_from_source(graphics::device& device, language lang, shader::stage stage, const std::string_view source, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<shader> auto_release() { return std::move(*this); }
	};
	static_assert(shader_concept<shader>);

	export struct pipeline : public graphics::pipeline { };
	export struct command_buffer: public graphics::command_buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_buffer); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_buffer);
		uint32_t type = magic_number;

		command_buffer(command_buffer&& o) { *this = std::move(o); }
		command_buffer& operator=(command_buffer&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		void submit(graphics::device& device, bool release = true) override { STYLIZER_API_THROW("Not implemented yet!"); }
		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<command_buffer> auto_release() { return std::move(*this); }
	};

	export struct bind_group: public graphics::bind_group { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(bind_group); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(bind_group);
		uint32_t type = magic_number;

		bind_group(bind_group&& o) { *this = std::move(o); }
		bind_group& operator=(bind_group&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<bind_group> auto_release() { return std::move(*this); }
	};

	export struct compute_pipeline: public graphics::compute_pipeline { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(compute_pipeline); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(compute_pipeline);
		uint32_t type = magic_number;

		compute_pipeline(compute_pipeline&& o) { *this = std::move(o); }
		compute_pipeline& operator=(compute_pipeline&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::compute_pipeline create(graphics::device& device, pipeline::entry_point entry_point, const std::string_view label = "Stylizer Compute Pipeline") { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::bind_group create_bind_group(graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<compute_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(compute_pipeline_concept<compute_pipeline>);

	export template<typename Tapi_return, typename Twebgpu_return>
	struct command_encoder_base : public Tapi_return { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder_base);
		uint32_t type = magic_number;
		std::shared_ptr<stylizer::signal<void()>> deferred_to_release = std::make_shared<stylizer::signal<void()>>();

		inline command_encoder_base(command_encoder_base&& o) { *this = std::move(o); }
		inline command_encoder_base& operator=(command_encoder_base&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override  { STYLIZER_API_THROW("Not implemented yet!"); }
		Twebgpu_return&& move() { return std::move(*(Twebgpu_return*)this); }


		static Twebgpu_return create(graphics::device& device, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") { STYLIZER_API_THROW("Not implemented yet!"); }

		template<typename Tfunc>
		Twebgpu_return& defer(Tfunc&& func) { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& defer(std::function<void()>&& func) override { STYLIZER_API_THROW("Not implemented yet!"); }

		Tapi_return& copy_buffer_to_buffer(graphics::device& device, graphics::buffer& destination, const graphics::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& copy_buffer_to_texture(graphics::device& device, graphics::buffer& destination, const graphics::texture& source, std::optional<size_t> destination_offset = 0, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_buffer(graphics::device& device, graphics::texture& destination, const graphics::buffer& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<size_t> source_offset = 0, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override  { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_texture(graphics::device& device, graphics::texture& destination, const graphics::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		Tapi_return& bind_compute_pipeline(graphics::device& device, const graphics::compute_pipeline& pipeline, bool release_on_submit = false) override { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& bind_compute_group(graphics::device& device, const graphics::bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& dispatch_workgroups(graphics::device& device, vec3u workgroups) override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::command_buffer end(graphics::device& device) { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::command_buffer& end(temporary_return_t, graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		void one_shot_submit(graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
	};

	export struct command_encoder : public command_encoder_base<graphics::command_encoder, command_encoder> { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_encoder);
		using super = stub::command_encoder_base<graphics::command_encoder, command_encoder>;
		inline command_encoder(command_encoder&& o) { *this = std::move(o); }
		inline command_encoder& operator=(command_encoder&& o) { STYLIZER_API_THROW("Not implemented yet!"); }

		stylizer::auto_release<command_encoder> auto_release() { return std::move(*this); }
	};
	static_assert(command_encoder_concept<command_encoder>);

	export namespace compute {
		using pipeline = compute_pipeline;
		using pass = command_encoder;
	}

	export struct render_pass : public command_encoder_base<graphics::render_pass, render_pass> { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pass); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pass);
		using super = stub::command_encoder_base<graphics::render_pass, render_pass>;

		// NOTE: Type gets inherited from command_encoder
		// inline render_pass(std::vector<color_attachment> colors, std::optional<depth_stencil_attachment> depth): color_attachments(std::move(colors)), depth_attachment(depth) {}
		inline render_pass(render_pass&& o) { *this = std::move(o); }
		inline render_pass& operator=(render_pass&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }


		static stub::render_pass create(graphics::device& device, std::span<const render_pass::color_attachment> colors, const std::optional<depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::render_pass& bind_render_pipeline(graphics::device& device, const graphics::render_pipeline& pipeline, bool release_on_submit =  false) override { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pass& bind_render_group(graphics::device& device, const graphics::bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pass& bind_vertex_buffer(graphics::device& device, size_t slot, const graphics::buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pass& bind_index_buffer(graphics::device& device, const graphics::buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::render_pass& draw(graphics::device& device, size_t vertex_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_vertex = 0, size_t first_instance = 0) override { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pass& draw_indexed(graphics::device& device, size_t index_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_index = 0, std::optional<size_t> base_vertex = 0, size_t first_instance = 0) override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::command_buffer end(graphics::device& device) { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::command_buffer& end(temporary_return_t, graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		void one_shot_submit(graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<render_pass> auto_release() { return std::move(*this); }
	};
	static_assert(render_pass_concept<render_pass>);

	export struct render_pipeline : public graphics::render_pipeline { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pipeline); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pipeline);
		uint32_t type = magic_number;

		render_pipeline(render_pipeline&& o) { *this = std::move(o); }
		render_pipeline& operator=(render_pipeline&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }


		static render_pipeline create(graphics::device& device, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline") { STYLIZER_API_THROW("Not implemented yet!"); }
		static render_pipeline create_from_compatible_render_pass(graphics::device& device, const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline") { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::bind_group create_bind_group(graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::bind_group& create_bind_group(temporary_return_t, graphics::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group") override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<render_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(render_pipeline_concept<render_pipeline>);

	export namespace render {
		using pipeline = render_pipeline;
		using pass = render_pass;
	}

	export struct surface : public graphics::surface { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(surface);  STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(surface);
		uint32_t type = magic_number;

		inline surface(surface&& o) { *this = std::move(o); }
		inline surface& operator=(surface&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::surface create_from_emscripten(const std::string_view canvas_id = "canvas") { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::surface create_from_cocoa(void* layer, void* window) { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::surface create_from_x11(void* display, void* window) { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::surface create_from_wayland(void* display, void* surface) { STYLIZER_API_THROW("Not implemented yet!"); }
		static stub::surface create_from_win32(void* window, void* instance) { STYLIZER_API_THROW("Not implemented yet!"); }

		config determine_optimal_default_config(graphics::device& device, vec2u size) override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::surface& configure(graphics::device& device, const config& config) override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::texture next_texture(graphics::device& device) { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::texture& next_texture(temporary_return_t, graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		texture::format configured_texture_format(graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		graphics::surface& present(graphics::device& device) override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<surface> auto_release() { return std::move(*this); }
	};
	static_assert(surface_concept<surface>);

	export struct device : public graphics::device { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(device); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(device);
		uint32_t type = magic_number;

		inline device(device&& o) { *this = std::move(o); }
		inline device& operator=(device&& o) { STYLIZER_API_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_API_THROW("Not implemented yet!"); }

		static stub::device create_default(const stub::device::create_config& config = {}) { STYLIZER_API_THROW("Not implemented yet!"); }

		bool tick(bool wait_for_queues = true) override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::texture create_texture(const graphics::texture::create_config& config = {}) { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::texture& create_texture(temporary_return_t, const graphics::texture::create_config& config = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		stub::texture create_and_write_texture(std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config = {}) { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const graphics::texture::data_layout& layout, const graphics::texture::create_config& config = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::buffer create_buffer(usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::buffer& create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") override { STYLIZER_API_THROW("Not implemented yet!"); }
		stub::buffer create_and_write_buffer(usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") override { STYLIZER_API_THROW("Not implemented yet!"); }
		template<typename T>
		requires(!std::same_as<T, std::byte>) // NOTE: non temp return version can't compile since buffer isn't a defined type yet!
		graphics::buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write_buffer(usage, byte_span(data), offset, label);
		}
		stub::shader create_shader_from_session(shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::shader& create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::shader create_shader_from_spirv(shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::shader& create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::shader create_shader_from_source(shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::shader& create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::command_encoder create_command_encoder(bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::command_encoder& create_command_encoder(temporary_return_t, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::render_pass create_render_pass(std::span<const graphics::render_pass::color_attachment> colors, std::optional<graphics::render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pass& create_render_pass(temporary_return_t, std::span<const graphics::render_pass::color_attachment> colors, const std::optional<graphics::render_pass::depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::compute_pipeline create_compute_pipeline(const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::compute_pipeline& create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") override { STYLIZER_API_THROW("Not implemented yet!"); }

		stub::render_pipeline create_render_pipeline(const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, std::optional<depth_stencil_attachment> depth_attachment = {}, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pipeline& create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override { STYLIZER_API_THROW("Not implemented yet!"); }
		stub::render_pipeline create_render_pipeline_from_compatible_render_pass(const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") { STYLIZER_API_THROW("Not implemented yet!"); }
		graphics::render_pipeline& create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const graphics::render_pass& compatible_render_pass, const graphics::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override { STYLIZER_API_THROW("Not implemented yet!"); }

		void release(bool static_sub_objects = false) override { STYLIZER_API_THROW("Not implemented yet!"); }
		stylizer::auto_release<device> auto_release() { return std::move(*this); }
	};
	static_assert(device_concept<device>);
} // namespace stylizer::graphics::stub