module;

#include "../../util/error_macros.hpp"
#include "../../util/method_macros.hpp"

#include <webgpu/webgpu.h>

#include <cassert>

export module stylizer.graphics.webgpu:buffer;

import :device;

namespace stylizer::graphics::webgpu {

    struct buffer : public graphics::buffer { STYLIZER_GRAPHICS_GENERIC_AUTO_RELEASE_SUPPORT(buffer); STYLIZER_GRAPHICS_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(buffer);
		uint32_t type = magic_number;
		WGPUBuffer buffer_ = nullptr;

		buffer(buffer&& o) { *this = std::move(o); }
		buffer& operator=(buffer&& o) {
			buffer_ = std::exchange(o.buffer_, nullptr);
			return *this;
		}
		inline operator bool() const override { return buffer_; }

		static buffer create(graphics::device& device_, enum usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer");

		static buffer create_and_write(graphics::device& device, enum usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer");
		
		template<typename T> requires(!std::same_as<T, std::byte>)
		static buffer create_and_write(graphics::device& device, enum usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			return create_and_write(device, usage, byte_span(data), offset, label);
		}

		static const buffer& zero_buffer(graphics::device& device, graphics::usage usage = usage::Storage, size_t minimum_size = 0, graphics::buffer* just_released = nullptr);
		const graphics::buffer& get_zero_buffer_singleton(graphics::device& device, enum usage usage = usage::Storage, size_t size = 0, graphics::buffer* just_released = nullptr) override;

		graphics::buffer& write(graphics::device& device_, std::span<const std::byte> data, size_t offset = 0) override;
		
		template<typename T> requires(!std::same_as<T, std::byte>)
		graphics::buffer& write(graphics::device& device, std::span<const T> data, size_t offset = 0) {
			return write(device, byte_span(data), offset);
		}

		size_t size() const override;

		enum usage usage() const override;

		graphics::buffer& copy_from(graphics::device& device_, const graphics::buffer& source_, std::optional<size_t> destination_offset = 0, std::optional<size_t> source_offset = 0, std::optional<size_t> size_override = {}) override;

		bool is_mapped() const override;

		std::future<std::byte*> map_async(graphics::device& device, std::optional<bool> for_writing_ = false, std::optional<size_t> offset_ = 0, std::optional<size_t> size_ = {}) override;

		std::byte* map(graphics::device& device, std::optional<bool> for_writing = false, std::optional<size_t> offset = 0, std::optional<size_t> size = {}) override;

		std::byte* get_mapped_range(bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) override;

		void unmap() override;

		void release() override;
		stylizer::auto_release<buffer> auto_release() { return std::move(*this); }
	};
	static_assert(buffer_concept<buffer>);

	export void copy_buffer_to_buffer_impl(WGPUCommandEncoder e, webgpu::buffer& destination, const webgpu::buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) {
		size_t size = std::min(destination.size() - destination_offset, source.size() - source_offset);
		assert(size_override.value_or(size) <= size);
		wgpuCommandEncoderCopyBufferToBuffer(e, source.buffer_, source_offset, destination.buffer_, destination_offset, size_override.value_or(size));
	}
}