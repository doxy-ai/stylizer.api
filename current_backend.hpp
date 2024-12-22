#pragma once

#if true // TODO: __EMSCRIPTEN__ // TODO: Replace with platform decision logic
	#include "webgpu/all.hpp"
	namespace stylizer::api::current_backend {
		using namespace webgpu;
	}
#endif