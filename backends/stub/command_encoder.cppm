module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:command_encoder;

import :device;
import :command_buffer;

namespace stylizer::graphics::stub {

	export template<typename Tapi_return, typename Tstub_return>
	struct command_encoder_base : public Tapi_return { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder_base);
		uint32_t type = magic_number;
		std::shared_ptr<stylizer::signal<void()>> deferred_to_release = std::make_shared<stylizer::signal<void()>>();

		inline command_encoder_base(command_encoder_base&& o) { *this = std::move(o); }
		inline command_encoder_base& operator=(command_encoder_base&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override  { STYLIZER_THROW("Not implemented yet!"); }
		Tstub_return&& move() { return std::move(*(Tstub_return*)this); }


		static Tstub_return create(graphics::device& device, bool one_shot = false, const std::string_view label = "Stylizer Command Encoder") { STYLIZER_THROW("Not implemented yet!"); }

		template<typename Tfunc>
		Tstub_return& defer(Tfunc&& func) { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& defer(std::function<void()>&& func) override { STYLIZER_THROW("Not implemented yet!"); }

		Tapi_return& copy_buffer_to_buffer(graphics::device& device, graphics::buffer& destination, const graphics::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& copy_buffer_to_texture(graphics::device& device, graphics::buffer& destination, const graphics::texture& source, std::optional<size_t> destination_offset = 0, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_buffer(graphics::device& device, graphics::texture& destination, const graphics::buffer& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<size_t> source_offset = 0, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override  { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& copy_texture_to_texture(graphics::device& device, graphics::texture& destination, const graphics::texture& source, std::optional<vec3u> destination_origin = { { 0, 0, 0 } }, std::optional<vec3u> source_origin = { { 0, 0, 0 } }, std::optional<vec3u> extent_override = {}, std::optional<size_t> min_mip_level = 0, std::optional<size_t> mip_levels_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }

		Tapi_return& bind_compute_pipeline(graphics::device& device, const graphics::compute_pipeline& pipeline, bool release_on_submit = false) override { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& bind_compute_group(graphics::device& device, const graphics::bind_group& group, std::optional<bool> release_on_submit = false, std::optional<size_t> index_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		Tapi_return& dispatch_workgroups(graphics::device& device, vec3u workgroups) override { STYLIZER_THROW("Not implemented yet!"); }

		stub::command_buffer end(graphics::device& device) { STYLIZER_THROW("Not implemented yet!"); }
		graphics::command_buffer& end(temporary_return_t, graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		void one_shot_submit(graphics::device& device) override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
	};

	export struct command_encoder : public command_encoder_base<graphics::command_encoder, command_encoder> { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(command_encoder); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(command_encoder);
		using super = stub::command_encoder_base<graphics::command_encoder, command_encoder>;
		inline command_encoder(command_encoder&& o) { *this = std::move(o); }
		inline command_encoder& operator=(command_encoder&& o) { STYLIZER_THROW("Not implemented yet!"); }

		stylizer::auto_release<command_encoder> auto_release() { return std::move(*this); }
	};
	static_assert(command_encoder_concept<command_encoder>);

	command_encoder device::create_command_encoder(bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::command_encoder& device::create_command_encoder(graphics::temporary_return_t, bool one_shot /* = false */, const std::string_view label /* = "Stylizer Command Encoder" */) { STYLIZER_THROW("Not implemented yet!"); }
}