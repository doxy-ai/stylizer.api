#include "common.hpp"

namespace stylizer::api::webgpu {
	inline usage from_webgpu(WGPUBufferUsage usage) {
		enum usage out = usage::Invalid;
		if(flags_set(usage, WGPUBufferUsage_MapRead)) out |= usage::MapRead;
		if(flags_set(usage,WGPUBufferUsage_MapWrite)) out |= usage::MapWrite;
		if(flags_set(usage,WGPUBufferUsage_CopySrc)) out |= usage::CopySource;
		if(flags_set(usage,WGPUBufferUsage_CopyDst)) out |= usage::CopyDestination;
		if(flags_set(usage,WGPUBufferUsage_Index)) out |= usage::Index;
		if(flags_set(usage,WGPUBufferUsage_Vertex)) out |= usage::Vertex;
		if(flags_set(usage,WGPUBufferUsage_Uniform)) out |= usage::Uniform;
		if(flags_set(usage,WGPUBufferUsage_Storage)) out |= usage::Storage;
		if(out == usage::Invalid) STYLIZER_API_THROW(std::string("Failed to find buffer usage: ") + std::to_string(usage));
		return out;
	};

	inline WGPUBufferUsage to_webgpu(usage usage) {
		WGPUBufferUsage out = {};
		if(flags_set(usage, usage::MapRead)) out |= WGPUBufferUsage_MapRead;
		if(flags_set(usage, usage::MapWrite)) out |= WGPUBufferUsage_MapWrite;
		if(flags_set(usage, usage::CopySource)) out |= WGPUBufferUsage_CopySrc;
		if(flags_set(usage, usage::CopyDestination)) out |= WGPUBufferUsage_CopyDst;
		if(flags_set(usage, usage::Index)) out |= WGPUBufferUsage_Index;
		if(flags_set(usage, usage::Vertex)) out |= WGPUBufferUsage_Vertex;
		if(flags_set(usage, usage::Uniform)) out |= WGPUBufferUsage_Uniform;
		if(flags_set(usage, usage::Storage)) out |= WGPUBufferUsage_Storage;
		if(out == 0) STYLIZER_API_THROW(std::string("Failed to find buffer usage: ") + std::string(magic_enum::enum_name(usage)));
		return out;
	}

	buffer buffer::create(api::device& device_, enum usage usage, size_t size, bool mapped_at_creation /* = false */, const std::string_view label /* = "Stylizer Buffer" */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);

		buffer out;
		WGPUBufferDescriptor d = WGPU_BUFFER_DESCRIPTOR_INIT;
		d.label = to_webgpu(label);
		d.usage = to_webgpu(usage);
		d.size = size;
		d.mappedAtCreation = mapped_at_creation;
		out.buffer_ = wgpuDeviceCreateBuffer(device.device_, &d);
		return out;
	}

	buffer buffer::create_and_write(api::device& device, enum usage usage, std::span<const std::byte> data, size_t offset /* = 0 */, const std::string_view label /* = "Stylizer Buffer" */) {
		auto out = create(device, usage | usage::CopyDestination, data.size(), false, label);
		out.write(device, data, offset);
		return out;
	}

	const buffer& buffer::zero_buffer(api::device& device, enum usage usage /* = usage::Storage */, size_t minimum_size /* = 0 */, api::buffer* just_released /* = nullptr */) {
		static constexpr auto create = [](api::device& device, enum usage usage, size_t minimum_size) {
			std::vector<std::byte> data(minimum_size, std::byte { 0 });
			return create_and_write(device, usage, data, 0, "Stylizer Zero Buffer");
		};
		static std::unordered_map<enum usage, stylizer::auto_release<buffer>> buffers;

		if (!buffers.contains(usage))
			buffers[usage] = create(device, usage, minimum_size);

		if (buffers[usage].size() < minimum_size) {
			if (just_released) confirm_webgpu_type<webgpu::buffer>(*just_released) = std::move(buffers[usage]);
			else buffers[usage].release();

			buffers[usage] = create(device, usage, minimum_size);
		}

		return buffers[usage];
	}
	const api::buffer& buffer::get_zero_buffer_singleton(api::device& device, enum usage usage /* = usage::Storage */, size_t size /* = 0 */, api::buffer* just_released /* = nullptr */) {
		return webgpu::buffer::zero_buffer(device, usage, size, just_released);
	}

	api::buffer& buffer::write(api::device& device_, std::span<const std::byte> data, size_t offset /* = 0 */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		wgpuQueueWriteBuffer(device.queue, buffer_, offset, data.data(), data.size());
		return *this;
	}

	size_t buffer::size() const { return wgpuBufferGetSize(buffer_); }

	enum usage buffer::usage() const { return from_webgpu(wgpuBufferGetUsage(buffer_)); }

	void copy_buffer_to_buffer_impl(WGPUCommandEncoder e, webgpu::buffer& destination, const webgpu::buffer& source, size_t destination_offset = 0, size_t source_offset = 0, std::optional<size_t> size_override = {}) {
		size_t size = std::min(destination.size() - destination_offset, source.size() - source_offset);
		assert(size_override.value_or(size) <= size);
		wgpuCommandEncoderCopyBufferToBuffer(e, source.buffer_, source_offset, destination.buffer_, destination_offset, size_override.value_or(size));
	}

	api::buffer& buffer::copy_from(api::device& device_, const api::buffer& source_, std::optional<size_t> destination_offset /* = 0 */, std::optional<size_t> source_offset /* = 0 */, std::optional<size_t> size_override /* = {} */) {
		auto& device = confirm_webgpu_type<webgpu::device>(device_);
		auto& source = confirm_webgpu_type<webgpu::buffer>(source_);
		auto e = create_command_encoder(device.device_);
		defer_ { wgpuCommandEncoderRelease(e); };
		copy_buffer_to_buffer_impl(e, *this, source, destination_offset.value_or(0), source_offset.value_or(0), size_override);
		finish_and_submit(e, device.queue);
		return *this;
	}

	bool buffer::is_mapped() const {
		// return buffer_.getMapState() == wgpu::BufferMapState::Mapped;
		assert(false && "Not Implemented in WEBGPU");
		return false;
	}

	std::future<std::byte*> buffer::map_async(api::device& device, std::optional<bool> for_writing_ /* = false */, std::optional<size_t> offset_ /* = 0 */, std::optional<size_t> size_ /* = {} */) {
		auto for_writing = for_writing_.value_or(false);
		auto offset = offset_.value_or(0);
		auto size = size_.value_or(this->size() - offset);
		struct userdata {
			std::promise<std::byte*> res;
			buffer* self;
			bool for_writing;
			size_t offset, size;
		};
		userdata* data = new userdata{{}, this, for_writing, offset, size};
		auto out = data->res.get_future();

		auto future = wgpuBufferMapAsync(buffer_, for_writing ? WGPUMapMode_Write : WGPUMapMode_Read, offset, size, {
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void*){
				struct userdata* data = (struct userdata*)userdata1;
				defer_ { delete data; };
				try {
					switch (status) {
					case WGPUMapAsyncStatus_CallbackCancelled: [[fallthrough]];
					case WGPUMapAsyncStatus_Error: [[fallthrough]];
					case WGPUMapAsyncStatus_Aborted:
						STYLIZER_API_THROW(from_webgpu(message));
					case WGPUMapAsyncStatus_Success: [[fallthrough]];
					case WGPUMapAsyncStatus_Force32:
						data->res.set_value(data->self->get_mapped_range(data->for_writing, data->offset, data->size));
					}
				} catch(...) {
					data->res.set_exception(std::current_exception());
				}
			}, .userdata1 = data, .userdata2 = nullptr
		});

		return out;
	}

	std::byte* buffer::map(api::device& device, std::optional<bool> for_writing /* = false */, std::optional<size_t> offset /* = 0 */, std::optional<size_t> size /* = {} */) {
		auto future = map_async(device, for_writing, offset, size);
		do {
			confirm_webgpu_type<webgpu::device>(device).process_events();
		} while(future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready);

		return future.get();
	}

	std::byte* buffer::get_mapped_range(bool for_writing /* = false */, size_t offset/*  = 0 */, std::optional<size_t> size /* = {} */) {
		// assert(is_mapped());
		if (for_writing) return (std::byte*)wgpuBufferGetMappedRange(buffer_, offset, size.value_or(this->size()));
		else return (std::byte*)wgpuBufferGetConstMappedRange(buffer_, offset, size.value_or(this->size()));
	}

	void buffer::unmap() {
		wgpuBufferUnmap(buffer_);
	}

	void buffer::release() {
		if (buffer_) wgpuBufferRelease(std::exchange(buffer_, nullptr));
	}

	static_assert(buffer_concept<buffer>);
} // namespace stylizer::api::webgpu