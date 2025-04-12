#ifdef STYLIZER_API_IMPLEMENTATION
#include "all.hpp"
#endif

#include "../api.h"

stylizer_api_device* stylizer_api_create_default_webgpu_device(stylizer_api_device_create_config* config) {
	return new stylizer::api::webgpu::device(stylizer::api::webgpu::device::create_default(*config));
}