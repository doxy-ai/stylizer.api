cmake_minimum_required(VERSION 3.20)
project(stylizer_api CXX)

set(STYLIZER_API_CURRENT_BACKEND_OPTIONS "webgpu" "none")
set(STYLIZER_API_CURRENT_BACKEND webgpu CACHE STRING "Backend that is currently set as the default for the Stylizer Graphics API")
set_property(CACHE STYLIZER_API_CURRENT_BACKEND PROPERTY STRINGS ${STYLIZER_API_CURRENT_BACKEND_OPTIONS})
string(TOLOWER ${STYLIZER_API_CURRENT_BACKEND} STYLIZER_API_CURRENT_BACKEND_LOWER)
if(NOT STYLIZER_API_CURRENT_BACKEND_LOWER IN_LIST STYLIZER_API_CURRENT_BACKEND_OPTIONS)
	message(FATAL_ERROR "The current backend must be one of ${STYLIZER_API_CURRENT_BACKEND_OPTIONS} not ${STYLIZER_API_CURRENT_BACKEND_LOWER}")
endif()


option(STYLIZER_API_ENABLE_WEBGPU "Should the Stylizer Graphics API support the WebGPU Backend?" ON)


set(CMAKE_CXX_STANDARD 20)

if(STYLIZER_API_ENABLE_WEBGPU)
	set(SLCROSS_ENABLE_DAWN ON CACHE BOOL "Should all of Dawn be built or just Tint?")
endif()
add_subdirectory(thirdparty/slcross EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/magic_enum EXCLUDE_FROM_ALL)

if(STYLIZER_API_ENABLE_WEBGPU)
	add_subdirectory(webgpu EXCLUDE_FROM_ALL)
elseif(STYLIZER_API_CURRENT_BACKEND_LOWER STREQUAL webgpu)
	message(FATAL_ERROR "WebGPU set as the current backend but it is not enabled!")
endif(STYLIZER_API_ENABLE_WEBGPU)

add_executable(api_tst "test.cpp")
target_link_libraries(api_tst PUBLIC stylizer::api::webgpu)

include(cmake/glfw.cmake)
target_setup_glfw_for_stylizer(api_tst)