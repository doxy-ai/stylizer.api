#pragma once

#include "../../api.hpp"
#include "../../util/string2magic.hpp"

#include <utility>
#include <webgpu/webgpu.h>

namespace stylizer::api::webgpu {
	constexpr static uint32_t magic_number = string2magic("WEBGPU");
	struct device;
	struct texture;

	struct texture_view : public api::texture_view { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(texture_view); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture_view);
		uint32_t type = magic_number;
		const webgpu::texture* owning_texture;
		WGPUTextureView view = nullptr;

		inline texture_view(texture_view&& o) { *this = std::move(o); }
		inline texture_view& operator=(texture_view&& o) {
			owning_texture = std::exchange(o.owning_texture, nullptr);
			view = std::exchange(o.view, nullptr);
			return *this;
		}
		inline operator bool() const override { return view; }

		static texture_view create(api::device& device, const api::texture& texture, const texture_view::create_config& config = {});

		const api::texture& texture() const override;

		void release() override;
		stylizer::auto_release<texture_view> auto_release() { return std::move(*this); }
	};
	static_assert(texture_view_concept<texture_view>);

	struct texture : public api::texture { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(texture); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(texture);
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

		static texture create(api::device& device, const create_config& config = {});
		static texture create_and_write(api::device& device, std::span<const std::byte> data, const data_layout& layout, create_config config = {});

		webgpu::texture_view create_view(api::device& device, const view::create_config& config = {}) const;
		api::texture_view& create_view(temporary_return_t, api::device& device, const view::create_config& config = {}) const override;
		const api::texture_view& full_view(api::device& device, bool treat_as_cubemap = false) const override;

		vec3u size() const override;
		enum texture_format texture_format() const override;
		enum usage usage() const override;
		uint32_t mip_levels() const override;
		uint32_t samples() const override;

		api::texture& configure_sampler(api::device& device, const sampler_config& config = {}) override;
		bool sampled() const override;

		api::texture& write(api::device& device, std::span<const std::byte> data, const data_layout& layout, vec3u extent, std::optional<vec3u> origin = { { 0, 0, 0 } }, size_t mip_level = 0) override;

		api::texture& copy_from(api::device& device, const api::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override;
		api::texture& blit_from(api::device& device, const api::texture& source, std::optional<color32> clear_value = {}, STYLIZER_NULLABLE api::render_pipeline* render_pipeline_override = nullptr, std::optional<size_t> vertex_count_override = {}) override;

		api::texture& generate_mipmaps(api::device& device, std::optional<size_t> first_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override;

		void release() override;
		stylizer::auto_release<texture> auto_release() { return std::move(*this); }
	};
	static_assert(texture_concept<texture>);

	struct buffer : public api::buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(buffer); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(buffer);
		uint32_t type = magic_number;
		WGPUBuffer buffer_ = nullptr;

		buffer(buffer&& o) { *this = std::move(o); }
		buffer& operator=(buffer&& o) {
			buffer_ = std::exchange(o.buffer_, nullptr);
			return *this;
		}
		inline operator bool() const override { return buffer_; }

		static buffer create(api::device& device, enum usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer");

		static buffer create_and_write(api::device& device, enum usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer");
		template<typename T>
		requires(!std::same_as<T, std::byte>)
		static buffer create_and_write(api::device& device, enum usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write(device, usage, byte_span(data), offset, label);
		}

		static const buffer& zero_buffer(api::device& device, enum usage usage = usage::Storage, size_t minimum_size = 0, api::buffer* just_released = nullptr);
		const api::buffer& get_zero_buffer_singleton(api::device& device, enum usage usage = usage::Storage, size_t size = 0, api::buffer* just_released = nullptr) override;

		api::buffer& write(api::device& device, std::span<const std::byte> data, size_t offset = 0) override;
		template<typename T>
		requires(!std::same_as<T, std::byte>)
		api::buffer& write(api::device& device, std::span<const T> data, size_t offset = 0) {
			return write(device, byte_span(data), offset);
		}

		size_t size() const override;
		enum usage usage() const override;

		api::buffer& copy_from(api::device& device, const api::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override;

		bool is_mapped() const override;

		std::future<std::byte*> map_async(api::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override;
		std::byte* map(api::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override;

		std::byte* get_mapped_range(bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {});

		void unmap() override;

		void release() override;
		stylizer::auto_release<buffer> auto_release() { return std::move(*this); }
	};
	static_assert(buffer_concept<buffer>);

	struct shader : public api::shader { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(shader); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(shader);
		uint32_t type = magic_number;
		WGPUShaderModule module = nullptr;
		// api::spirv spirv = {}; // TODO: Can we store some sort of smaller reflection data?

		shader(shader&& o) { *this = std::move(o); }
		shader& operator=(shader&& o) {
			module = std::exchange(o.module, nullptr);
			// spirv = std::exchange(o.spirv, {});
			return *this;
		}
		inline operator bool() const override { return module; }

		static shader create_from_wgsl(api::device& device, const std::string_view wgsl, const std::string_view label = "Stylizer Shader");
		static shader create_from_session(api::device& device, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");
		static shader create_from_spirv(api::device& device, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");
		static shader create_from_source(api::device& device, language lang, shader::stage stage, const std::string_view source, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");

		void release() override;
		stylizer::auto_release<shader> auto_release() { return std::move(*this); }
	};
	static_assert(shader_concept<shader>);

	struct pipeline : public api::pipeline { };
	struct command_buffer: public api::command_buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_buffer); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_buffer);
		uint32_t type = magic_number;
		WGPUCommandBuffer pre = nullptr;
		WGPUCommandBuffer compute = nullptr;
		WGPUCommandBuffer render = nullptr;
		// TODO: Do we want to support sub command buffers?

		command_buffer(command_buffer&& o) { *this = std::move(o); }
		command_buffer& operator=(command_buffer&& o) {
			deferred_to_release = std::move(o.deferred_to_release);
			pre = std::exchange(o.pre, nullptr);
			compute = std::exchange(o.compute, nullptr);
			render = std::exchange(o.render, nullptr);
			return *this;
		}
		inline operator bool() const override { return pre || compute || render; }

		void submit(api::device& device, bool release = true) override;
		void release() override;
		stylizer::auto_release<command_buffer> auto_release() { return std::move(*this); }
	};

	struct bind_group: public api::bind_group { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(bind_group); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(bind_group);
		uint32_t type = magic_number;
		WGPUBindGroup group = nullptr;
		size_t index = 0;

		bind_group(bind_group&& o) { *this = std::move(o); }
		bind_group& operator=(bind_group&& o) {
			group = std::exchange(o.group, nullptr);
			index = o.index;
			return *this;
		}
		inline operator bool() const override { return group; }

		void release() override {
			if(group) wgpuBindGroupRelease(std::exchange(group, nullptr));
		}
		stylizer::auto_release<bind_group> auto_release() { return std::move(*this); }
	};

	struct compute_pipeline: public api::compute_pipeline { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(compute_pipeline); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(compute_pipeline);
		uint32_t type = magic_number;
		WGPUComputePipeline pipeline = nullptr;

		compute_pipeline(compute_pipeline&& o) { *this = std::move(o); }
		compute_pipeline& operator=(compute_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() const override { return pipeline; }

		static compute_pipeline create(api::device& device, pipeline::entry_point entry_point, const std::string_view label = "Stylizer Compute Pipeline");

		webgpu::bind_group create_bind_group(api::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group");
		api::bind_group& create_bind_group(temporary_return_t, api::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Bind Group") override;

		void release() override;
		stylizer::auto_release<compute_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(compute_pipeline_concept<compute_pipeline>);

	template<typename Tapi_return, typename Twebgpu_return>
	struct command_encoder_base : public Tapi_return { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder_base);
		uint32_t type = magic_number;
		std::shared_ptr<stylizer::signal<void()>> deferred_to_release = std::make_shared<stylizer::signal<void()>>();
		WGPUCommandEncoder pre_encoder = nullptr;
		WGPUCommandEncoder compute_encoder = nullptr;
		WGPUComputePassEncoder compute_pass = nullptr;
		std::string label;
		bool one_shot = false;

		inline command_encoder_base(command_encoder_base&& o) { *this = std::move(o); }
		inline command_encoder_base& operator=(command_encoder_base&& o) {
			deferred_to_release = std::move(o.deferred_to_release);
			pre_encoder = std::exchange(o.pre_encoder, nullptr);
			compute_encoder = std::exchange(o.compute_encoder, nullptr);
			compute_pass = std::exchange(o.compute_pass, nullptr);
			label = std::move(o.label);
			one_shot = o.one_shot;
			return *this;
		}
		inline operator bool() const override { return pre_encoder || compute_encoder || compute_pass; }
		Twebgpu_return&& move() { return std::move(*(Twebgpu_return*)this); }

		static Twebgpu_return create(api::device& device, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder");

		template<typename Tfunc>
		Twebgpu_return& defer(Tfunc&& func) { deferred_to_release->connect(std::move(func)); return *(Twebgpu_return*)this; }
		Tapi_return& defer(std::function<void()>&& func) override { deferred_to_release->connect(std::move(func)); return *(Tapi_return*)this; }

	protected:
		WGPUCommandEncoder maybe_create_pre_encoder(webgpu::device& device);
		WGPUComputePassEncoder maybe_create_compute_pass(webgpu::device& device);
	public:

		Tapi_return& copy_buffer_to_buffer(api::device& device, api::buffer& destination, const api::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override;
		Tapi_return& copy_buffer_to_texture(api::device& device, api::buffer& destination, const api::texture& source, std::optional<size_t> destination_offset = 0, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_buffer(api::device& device, api::texture& destination, const api::buffer& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<size_t> source_offset = 0, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override  { STYLIZER_API_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_texture(api::device& device, api::texture& destination, const api::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override;

		Tapi_return& bind_compute_pipeline(api::device& device, const api::compute_pipeline& pipeline, bool release_on_submit = false) override;
		Tapi_return& bind_compute_group(api::device& device, const api::bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override;
		Tapi_return& dispatch_workgroups(api::device& device, vec3u workgroups) override;

		webgpu::command_buffer end(api::device& device);
		api::command_buffer& end(temporary_return_t, api::device& device) override {
			static command_buffer buffer;
			return buffer = end(device);
		}

		void one_shot_submit(api::device& device) override;

		void release() override;
	};

	struct command_encoder : public command_encoder_base<api::command_encoder, command_encoder> { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_encoder);
		using super = webgpu::command_encoder_base<api::command_encoder, command_encoder>;
		inline command_encoder(command_encoder&& o) { *this = std::move(o); }
		inline command_encoder& operator=(command_encoder&& o) { super::operator=(std::move(o)); return *this; }

		stylizer::auto_release<command_encoder> auto_release() { return std::move(*this); }
	};
	static_assert(command_encoder_concept<command_encoder>);

	namespace compute {
		using pipeline = compute_pipeline;
		using pass = command_encoder;
	}

	struct render_pass : public command_encoder_base<api::render_pass, render_pass> { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pass); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pass);
		using super = webgpu::command_encoder_base<api::render_pass, render_pass>;

		// NOTE: Type gets inherited from command_encoder
		WGPUCommandEncoder render_encoder = nullptr;
		WGPURenderPassEncoder pass = nullptr;
		std::vector<color_attachment> color_attachments = {};
		std::optional<depth_stencil_attachment> depth_attachment = {};
		bool render_used = false;

		inline render_pass(std::vector<color_attachment> colors, std::optional<depth_stencil_attachment> depth): color_attachments(std::move(colors)), depth_attachment(depth) {}
		inline render_pass(render_pass&& o) { *this = std::move(o); }
		inline render_pass& operator=(render_pass&& o) {
			super::operator=(std::move(o));
			render_encoder = std::exchange(o.render_encoder, nullptr);
			pass = std::exchange(o.pass, nullptr);
			color_attachments = std::exchange(o.color_attachments, {});
			depth_attachment = std::exchange(o.depth_attachment, {});
			render_used = std::exchange(o.render_used, false);
			return *this;
		}
		inline operator bool() const override { return super::operator bool() || render_encoder || pass; }

		static render_pass create(api::device& device, std::span<const render_pass::color_attachment> colors, const std::optional<depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass");

		api::render_pass& bind_render_pipeline(api::device& device, const api::render_pipeline& pipeline, bool release_on_submit =  false) override;
		api::render_pass& bind_render_group(api::device& device, const api::bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override;
		api::render_pass& bind_vertex_buffer(api::device& device, size_t slot, const api::buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override;
		api::render_pass& bind_index_buffer(api::device& device, const api::buffer& buffer, std::optional<size_t> offset = 0, std::optional<size_t> size_override = {}) override;

		api::render_pass& draw(api::device& device, size_t vertex_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_vertex = 0, size_t first_instance = 0) override;
		api::render_pass& draw_indexed(api::device& device, size_t index_count, std::optional<size_t> instance_count = 1, std::optional<size_t> first_index = 0, std::optional<size_t> base_vertex = 0, size_t first_instance = 0) override;

		webgpu::command_buffer end(api::device& device);
		api::command_buffer& end(temporary_return_t, api::device& device) override;

		void one_shot_submit(api::device& device) override;

		void release() override;
		stylizer::auto_release<render_pass> auto_release() { return std::move(*this); }
	};
	static_assert(render_pass_concept<render_pass>);

	struct render_pipeline : public api::render_pipeline { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(render_pipeline); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(render_pipeline);
		uint32_t type = magic_number;
		WGPURenderPipeline pipeline = nullptr;

		render_pipeline(render_pipeline&& o) { *this = std::move(o); }
		render_pipeline& operator=(render_pipeline&& o) {
			pipeline = std::exchange(o.pipeline, nullptr);
			return *this;
		}
		inline operator bool() const override { return pipeline; }

		static render_pipeline create(api::device& device, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline");
		static render_pipeline create_from_compatible_render_pass(api::device& device, const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const render_pipeline::config& config = {}, const std::string_view label = "Stylizer Graphics Pipeline");

		webgpu::bind_group create_bind_group(api::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group");
		api::bind_group& create_bind_group(temporary_return_t, api::device& device, size_t index, std::span<const bind_group::binding> bindings, std::string_view label = "Stylizer Render Bind Group") override;

		void release() override;
		stylizer::auto_release<render_pipeline> auto_release() { return std::move(*this); }
	};
	static_assert(render_pipeline_concept<render_pipeline>);

	namespace render {
		using pipeline = render_pipeline;
		using pass = render_pass;
	}

	struct surface : public api::surface { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(surface);  STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(surface);
		uint32_t type = magic_number;
		WGPUSurface surface_ = nullptr;
		texture::format configured_format = texture::format::Undefined;

		inline surface(surface&& o) { *this = std::move(o); }
		inline surface& operator=(surface&& o) {
			surface_ = std::exchange(o.surface_, nullptr);
			configured_format = std::exchange(o.configured_format, texture::format::Undefined);
			return *this;
		}
		inline operator bool() const override { return surface_; }

		static webgpu::surface create_from_emscripten(const std::string_view canvas_id = "canvas");
		static webgpu::surface create_from_cocoa(void* layer, void* window);
		static webgpu::surface create_from_x11(void* display, void* window);
		static webgpu::surface create_from_wayland(void* display, void* surface);
		static webgpu::surface create_from_win32(void* window, void* instance);

		config determine_optimal_default_config(api::device& device, vec2u size) override;

		api::surface& configure(api::device& device, const config& config) override;

		webgpu::texture next_texture(api::device& device);
		api::texture& next_texture(temporary_return_t, api::device& device) override;

		texture::format configured_texture_format(api::device& device) override;

		api::surface& present(api::device& device) override;

		void release() override;
		stylizer::auto_release<surface> auto_release() { return std::move(*this); }
	};
	static_assert(surface_concept<surface>);

	struct device : public api::device { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(device); STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(device);
		uint32_t type = magic_number;
		WGPUAdapter adapter = nullptr;
		WGPUDevice device_ = nullptr;
		WGPUQueue queue = nullptr;

		inline device(device&& o) { *this = std::move(o); }
		inline device& operator=(device&& o) {
			adapter = std::exchange(o.adapter, nullptr);
			device_ = std::exchange(o.device_, nullptr);
			queue = std::exchange(o.queue, nullptr);
			return *this;
		}
		inline operator bool() const override { return adapter || device_; }

		static webgpu::device create_default(const webgpu::device::create_config& config = {});

		bool process_events();
		bool tick(bool wait_for_queues = true) override;

		webgpu::texture create_texture(const api::texture::create_config& config = {});
		api::texture& create_texture(temporary_return_t, const api::texture::create_config& config = {}) override;
		webgpu::texture create_and_write_texture(std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config = {});
		api::texture& create_and_write_texture(temporary_return_t, std::span<const std::byte> data, const api::texture::data_layout& layout, const api::texture::create_config& config = {}) override;

		webgpu::buffer create_buffer(usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer");
		api::buffer& create_buffer(temporary_return_t, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") override;
		webgpu::buffer create_and_write_buffer(usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer");
		api::buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") override;
		template<typename T>
		requires(!std::same_as<T, std::byte>) // NOTE: non temp return version can't compile since buffer isn't a defined type yet!
		api::buffer& create_and_write_buffer(temporary_return_t, usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write_buffer(usage, byte_span(data), offset, label);
		}

		webgpu::shader create_shader_from_wgsl(const std::string_view wgsl, const std::string_view label = "Stylizer Shader");
		api::shader& create_shader_from_wgsl(temporary_return_t, const std::string_view wgsl, const std::string_view label = "Stylizer Shader");

		webgpu::shader create_shader_from_session(shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");
		api::shader& create_shader_from_session(temporary_return_t, shader::stage stage, slcross::session session, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") override;
		webgpu::shader create_shader_from_spirv(shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader");
		api::shader& create_shader_from_spirv(temporary_return_t, shader::stage stage, spirv_view spirv, const std::string_view entry_point = "main", const std::string_view label = "Stylizer Shader") override;
		webgpu::shader create_shader_from_source(shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader");
		api::shader& create_shader_from_source(temporary_return_t, shader::language lang, shader::stage stage, const std::string_view source, std::optional<const std::string_view> entry_point_ = "main", const std::string_view label = "Stylizer Shader") override;

		webgpu::command_encoder create_command_encoder(bool one_shot = false, const std::string_view label = "Stylizer Command Encoder");
		api::command_encoder& create_command_encoder(temporary_return_t, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") override;

		webgpu::render_pass create_render_pass(std::span<const api::render_pass::color_attachment> colors, std::optional<api::render_pass::depth_stencil_attachment> depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass");
		api::render_pass& create_render_pass(temporary_return_t, std::span<const api::render_pass::color_attachment> colors, const std::optional<api::render_pass::depth_stencil_attachment>& depth = {}, bool one_shot = false, const std::string_view label = "Stylizer Render Pass") override;

		webgpu::compute_pipeline create_compute_pipeline(const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline");
		api::compute_pipeline& create_compute_pipeline(temporary_return_t, const pipeline::entry_point& entry_point, const std::string_view label = "Stylizer Compute Pipeline") override;

		webgpu::render_pipeline create_render_pipeline(const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, std::optional<depth_stencil_attachment> depth_attachment = {}, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline");
		api::render_pipeline& create_render_pipeline(temporary_return_t, const pipeline::entry_points& entry_points, std::span<const color_attachment> color_attachments = {}, const std::optional<depth_stencil_attachment>& depth_attachment = {}, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override;
		webgpu::render_pipeline create_render_pipeline_from_compatible_render_pass(const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline");
		api::render_pipeline& create_render_pipeline_from_compatible_render_pass(temporary_return_t, const pipeline::entry_points& entry_points, const api::render_pass& compatible_render_pass, const api::render_pipeline::config& config = {}, const std::string_view label = "Stylizer Render Pipeline") override;

		void release(bool static_sub_objects = false) override;
		stylizer::auto_release<device> auto_release() { return std::move(*this); }
	};
} // namespace stylizer::api::stub