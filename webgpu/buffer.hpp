#pragma once

#include "cstring_from_view.hpp"
#include "device.hpp"

namespace stylizer::api::webgpu {
	struct buffer: public api::buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(buffer);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::Buffer buffer_;

		buffer(buffer&& o) { *this = std::move(o); }
		buffer& operator=(buffer&& o) {
			buffer_ = std::exchange(o.buffer_, nullptr);
			return *this;
		}
		inline operator bool() { return buffer_; }

		static buffer create(api::device& device_, usage usage, size_t size, bool mapped_at_creation = false, std::string_view label = "Stylizer Buffer"){
			auto& device = confirm_wgpu_type<webgpu::device>(device_);

			buffer out;
			out.buffer_ = device.device_.createBuffer(WGPUBufferDescriptor {
				.label = cstring_from_view(label),
				.usage = to_wgpu_buffer(usage),
				.size = size,
				.mappedAtCreation = mapped_at_creation
			});
			return out;
		}

		static buffer create_and_write(api::device& device, usage usage, std::span<std::byte> data, size_t offset = 0, std::string_view label = "Stylizer Buffer") {
			auto out = create(device, usage | usage::CopyDestination, data.size(), false, label);
			out.write(device, data, offset);
			return out;
		}
		template<typename T> requires(!std::same_as<T, std::byte>)
		static buffer create_and_write(api::device& device, usage usage, std::span<T> data, size_t offset = 0, std::string_view label = "Stylizer Buffer") {
			return create_and_write(device, usage, byte_span(data), offset, label);
		}

		static const buffer& zero_buffer(api::device& device, usage usage = usage::Storage, size_t minimum_size = 0, api::buffer* just_released = nullptr){
			static constexpr auto create = [](api::device& device, enum usage usage, size_t minimum_size) {
				std::vector<std::byte> data(minimum_size, std::byte{0});
				return create_and_write(device, usage, data, 0, "Stylizer Zero Buffer");
			};
			static std::unordered_map<enum usage, auto_release<buffer>> buffers;

			if(!buffers.contains(usage))
				buffers[usage] = create(device, usage, minimum_size);

			if(buffers[usage].size(device) < minimum_size) {
				if(just_released) confirm_wgpu_type<webgpu::buffer>(*just_released) = std::move(buffers[usage]);
				else buffers[usage].release();

				buffers[usage] = create(device, usage, minimum_size);
			}

			return buffers[usage];
		}
		const api::buffer& get_zero_buffer_singleton(api::device& device, usage usage = usage::Storage, size_t size = 0, api::buffer* just_released = nullptr) override {
			return webgpu::buffer::zero_buffer(device, usage, size, just_released);
		}

		api::buffer& write(api::device& device_, std::span<std::byte> data, size_t offset = 0) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			device.queue.writeBuffer(buffer_, offset, data.data(), data.size());
			return *this;
		}
		template<typename T> requires(!std::same_as<T, std::byte>)
		api::buffer& write(api::device& device, std::span<T> data, size_t offset = 0) {
			return write(device, byte_span(data), offset);
		}

		size_t size(api::device& device_) override { return buffer_.getSize(); }

		void release() override {
			if(buffer_) std::exchange(buffer_, nullptr).release();
		}
	};
	static_assert(buffer_concept<buffer>);
}