#pragma once

#if STYLIZER_CURRENT_BACKEND == 1 // TODO: Replace with platform decision logic
	#include "webgpu/webgpu.hpp"
	namespace stylizer::api::current_backend {
		using namespace webgpu;
	}
#endif