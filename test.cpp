#define WEBGPU_CPP_IMPLEMENTATION
#include "wgpu/device.hpp"

#include <iostream>

int main() {
	auto device = stylizer::api::wgpu::device::create_default({.high_performance = true});

	

	device.release(true);
}