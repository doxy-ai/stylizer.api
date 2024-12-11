cmake_minimum_required(VERSION 3.20)
project(stylizer_api CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(thirdparty/slcross)
add_subdirectory(thirdparty/WebGPU-distribution)
add_subdirectory(thirdparty/magic_enum)

add_executable(tst "test.cpp" "webgpu/device.cpp" "webgpu/render_pass.cpp" "webgpu/texture.cpp")
target_link_libraries(tst PUBLIC magic_enum slcross::slcross webgpu)

include(cmake/glfw.cmake)
target_setup_glfw_for_stylizer(tst)