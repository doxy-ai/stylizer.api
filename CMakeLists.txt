cmake_minimum_required(VERSION 3.20)
project(stylizer_api CXX)

add_subdirectory(thirdparty/slcross)
add_subdirectory(thirdparty/WebGPU-distribution)

add_executable(tst "test.cpp")
target_link_libraries(tst PUBLIC slcross::slcross webgpu)