module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

export module stylizer.graphics.stub:buffer;

import :device;

namespace stylizer::graphics::stub {

    export struct buffer : public graphics::buffer { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(buffer); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(buffer);
		uint32_t type = magic_number;

		buffer(buffer&& o) { *this = std::move(o); }
		buffer& operator=(buffer&& o) { STYLIZER_THROW("Not implemented yet!"); }
		inline operator bool() const override { STYLIZER_THROW("Not implemented yet!"); }

		static stub::buffer create(graphics::device& device, enum usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer") { STYLIZER_THROW("Not implemented yet!"); }

		static stub::buffer create_and_write(graphics::device& device, enum usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") { STYLIZER_THROW("Not implemented yet!"); }
		template<typename T>
		requires(!std::same_as<T, std::byte>)
		static stub::buffer create_and_write(graphics::device& device, enum usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write(device, usage, byte_span(data), offset, label);
		}

		static const stub::buffer& zero_buffer(graphics::device& device, enum usage usage = usage::Storage, size_t minimum_size = 0, graphics::buffer* just_released = nullptr) { STYLIZER_THROW("Not implemented yet!"); }
		const graphics::buffer& get_zero_buffer_singleton(graphics::device& device, enum usage usage = usage::Storage, size_t size = 0, graphics::buffer* just_released = nullptr) override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::buffer& write(graphics::device& device, std::span<const std::byte> data, size_t offset = 0) override { STYLIZER_THROW("Not implemented yet!"); }
		template<typename T>
		requires(!std::same_as<T, std::byte>)
		graphics::buffer& write(graphics::device& device, std::span<const T> data, size_t offset = 0) {
			return write(device, byte_span(data), offset);
		}

		size_t size() const override { STYLIZER_THROW("Not implemented yet!"); }
		enum usage usage() const override { STYLIZER_THROW("Not implemented yet!"); }

		graphics::buffer& copy_from(graphics::device& device, const graphics::buffer& source, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override { STYLIZER_THROW("Not implemented yet!"); }

		bool is_mapped() const override { STYLIZER_THROW("Not implemented yet!"); }

		std::future<std::byte*> map_async(graphics::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override { STYLIZER_THROW("Not implemented yet!"); }
		std::byte* map(graphics::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override { STYLIZER_THROW("Not implemented yet!"); }

		std::byte* get_mapped_range(bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) { STYLIZER_THROW("Not implemented yet!"); }

		void unmap() override { STYLIZER_THROW("Not implemented yet!"); }

		void release() override { STYLIZER_THROW("Not implemented yet!"); }
		stylizer::auto_release<buffer> auto_release() { return std::move(*this); }
	};
	static_assert(buffer_concept<buffer>);

	stub::buffer device::create_buffer(graphics::usage usage, size_t size, bool mapped_at_creation /* = false */, const std::string_view label /* = "Stylizer Buffer" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::buffer& device::create_buffer(graphics::temporary_return_t, graphics::usage usage, size_t size, bool mapped_at_creation /* = false */, const std::string_view label /* = "Stylizer Buffer" */) { STYLIZER_THROW("Not implemented yet!"); }
	stub::buffer device::create_and_write_buffer(graphics::usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) { STYLIZER_THROW("Not implemented yet!"); }
	graphics::buffer& device::create_and_write_buffer(graphics::temporary_return_t, graphics::usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) { STYLIZER_THROW("Not implemented yet!"); }
	template<typename T> requires(!std::same_as<T, std::byte>)
	stub::buffer device::create_and_write_buffer(graphics::usage usage, std::span<const T> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		return create_and_write_buffer(usage, byte_span(data), offset, label);
	}
}