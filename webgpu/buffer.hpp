#pragma once

#include "cstring_from_view.hpp"
#include "device.hpp"
#include <webgpu/webgpu.hpp>

namespace stylizer::api::webgpu {
	struct buffer: public api::buffer { STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(buffer);
		char type[4] = STYLIZER_API_WGPU_TYPE;
		wgpu::Buffer buffer_;

		buffer(buffer&& o) { *this = std::move(o); }
		buffer& operator=(buffer&& o) {
			buffer_ = std::exchange(o.buffer_, nullptr);
			return *this;
		}
		inline operator bool() const override { return buffer_; }

		static buffer create(api::device& device_, usage usage, size_t size, bool mapped_at_creation = false, const std::string_view label = "Stylizer Buffer"){
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

		static buffer create_and_write(api::device& device, usage usage, std::span<const std::byte> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
			auto out = create(device, usage | usage::CopyDestination, data.size(), false, label);
			out.write(device, data, offset);
			return out;
		}
		template<typename T> requires(!std::same_as<T, std::byte>)
		static buffer create_and_write(api::device& device, usage usage, std::span<const T> data, size_t offset = 0, const std::string_view label = "Stylizer Buffer") {
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

			if(buffers[usage].size() < minimum_size) {
				if(just_released) confirm_wgpu_type<webgpu::buffer>(*just_released) = std::move(buffers[usage]);
				else buffers[usage].release();

				buffers[usage] = create(device, usage, minimum_size);
			}

			return buffers[usage];
		}
		const api::buffer& get_zero_buffer_singleton(api::device& device, usage usage = usage::Storage, size_t size = 0, api::buffer* just_released = nullptr) override {
			return webgpu::buffer::zero_buffer(device, usage, size, just_released);
		}

		api::buffer& write(api::device& device_, std::span<const std::byte> data, size_t offset = 0) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			device.queue.writeBuffer(buffer_, offset, data.data(), data.size());
			return *this;
		}
		template<typename T> requires(!std::same_as<T, std::byte>)
		api::buffer& write(api::device& device, std::span<const T> data, size_t offset = 0) {
			return write(device, byte_span(data), offset);
		}

		size_t size() const override { return const_cast<wgpu::Buffer&>(buffer_).getSize(); }



		static inline void copy_buffer_to_buffer_impl(wgpu::CommandEncoder e, webgpu::buffer& destination, const webgpu::buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) {
			size_t size = std::min(destination.size() - destination_offset, source.size() - source_offset);
			assert(size_override.value_or(size) <= size);
			e.copyBufferToBuffer(source.buffer_, source_offset, destination.buffer_, destination_offset, size_override.value_or(size));
		}

		api::buffer& copy_from(api::device& device_, const api::buffer& source_, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) override {
			auto& device = confirm_wgpu_type<webgpu::device>(device_);
			auto& source = confirm_wgpu_type<webgpu::buffer>(source_);
			auto_release e = device.device_.createCommandEncoder();
			copy_buffer_to_buffer_impl(e, *this, source, destination_offset, source_offset, size_override);
			auto_release commands = e.finish();
			device.queue.submit(commands);
			return *this;
		}



		bool is_mapped() const override {
			// return buffer_.getMapState() == wgpu::BufferMapState::Mapped;
			assert(false && "Not Implemented in WEBGPU");
		}

		std::unique_ptr<wgpu::BufferMapCallback>& get_async_map_callback() {
			static std::unique_ptr<wgpu::BufferMapCallback> callback;
			return callback;
		}

		std::future<std::byte*> map_async(api::device& device, bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) override {
			static std::promise<std::byte*> res; res = {};
			static constexpr auto set_promise_value = [](std::promise<std::byte*>& res, wgpu::Buffer buffer, size_t offset, size_t size, bool for_writing) {
				if(for_writing) res.set_value((std::byte*)buffer.getMappedRange(offset, size));
				else res.set_value((std::byte*)buffer.getConstMappedRange(offset, size));
			};

			// if(is_mapped()) {
			// 	set_promise_value(res, buffer_, offset, size.value_or(this->size() - offset), for_writing);
			// 	return res.get_future();
			// }

			auto callback = [for_writing, this, offset, size](wgpu::BufferMapAsyncStatus status) mutable {
				assert(status == WGPUBufferMapAsyncStatus_Success);
				set_promise_value(res, buffer_, offset, size.value_or(this->size() - offset), for_writing);
			};
			get_async_map_callback() = buffer_.mapAsync(for_writing ? wgpu::MapMode::Write : wgpu::MapMode::Read, offset, size.value_or(this->size() - offset), callback);
			return res.get_future();
		}

		std::byte* map(api::device& device, bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) override {
			auto future = map_async(device, for_writing, offset, size);
			confirm_wgpu_type<webgpu::device>(device).process_events();
			return future.get();
		}

		std::byte* get_mapped_range(bool for_writing = false, size_t offset = 0, std::optional<size_t> size = {}) {
			// assert(is_mapped());
			if(for_writing) return (std::byte*)buffer_.getMappedRange(offset, size.value_or(this->size()));
			else return (std::byte*)buffer_.getConstMappedRange(offset, size.value_or(this->size()));
		}

		void unmap() override {
			buffer_.unmap();
		}

		void release() override {
			if(buffer_) std::exchange(buffer_, nullptr).release();
		}
	};
	static_assert(buffer_concept<buffer>);
}