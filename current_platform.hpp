#pragma once

#if true // TODO: __EMSCRIPTEN__ // TODO: Replace with platform decision logic
	#include "webgpu/all.hpp"
	namespace stylizer::api::current_platform {
		using namespace webgpu;
	}
#endif