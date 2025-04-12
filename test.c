#define STYLIZER_API_IMPLEMENTATION
#include "webgpu/c.h"
#include "glfw.hpp"

int main() {
	glfwInit();

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Stylizer Test", NULL, NULL);

	auto device_config = stylizer_api_device_create_config_create_default();
	device_config.high_performance = true;
	auto device = stylizer_api_create_default_webgpu_device(&device_config);
}