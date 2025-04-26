cmake_minimum_required(VERSION 3.21)
project(stylizer_api CXX)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

set(STYLIZER_API_CURRENT_BACKEND_OPTIONS "webgpu" "none")
set(STYLIZER_API_CURRENT_BACKEND webgpu CACHE STRING "Backend that is currently set as the default for the Stylizer Graphics API")
set_property(CACHE STYLIZER_API_CURRENT_BACKEND PROPERTY STRINGS ${STYLIZER_API_CURRENT_BACKEND_OPTIONS})
string(TOLOWER ${STYLIZER_API_CURRENT_BACKEND} STYLIZER_API_CURRENT_BACKEND_LOWER)
if(NOT STYLIZER_API_CURRENT_BACKEND_LOWER IN_LIST STYLIZER_API_CURRENT_BACKEND_OPTIONS)
	message(FATAL_ERROR "The current backend must be one of ${STYLIZER_API_CURRENT_BACKEND_OPTIONS} not ${STYLIZER_API_CURRENT_BACKEND_LOWER}")
endif()

# option(STYLIZER_API_BUILD_TEST "Should we build the test file?" ${PROJECT_IS_TOP_LEVEL})

option(STYLIZER_USE_COCOA "Should we build cocoa support?" ${APPLE})
option(STYLIZER_USE_WINDOWS_UI "Should we build windows ui support?" ${WIN32})
option(STYLIZER_USE_X11 "Should we build X11 support?" ${LINUX})
option(STYLIZER_USE_WAYLAND "Should we build wayland support?" OFF)

# option(STYLIZER_API_ENABLE_WEBGPU "Should the Stylizer Graphics API support the WebGPU Backend?" ON)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(thirdparty/slcross EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/stdmath EXCLUDE_FROM_ALL)

add_library(stylizer_api api.cpp)
target_link_libraries(stylizer_api PUBLIC slcross magic_enum std::math)
target_include_directories(stylizer_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/backends)

# if(STYLIZER_API_ENABLE_WEBGPU)
# 	add_subdirectory(backends/webgpu EXCLUDE_FROM_ALL)
# elseif(STYLIZER_API_CURRENT_BACKEND_LOWER STREQUAL webgpu)
# 	message(FATAL_ERROR "WebGPU set as the current backend but it is not enabled!")
# endif(STYLIZER_API_ENABLE_WEBGPU)

# if(${STYLIZER_API_BUILD_TEST})
# 	find_package(SDL3 REQUIRED)

# 	add_executable(api_tst "test.cpp")
# 	target_link_libraries(api_tst PUBLIC stylizer::api::webgpu)

# 	include(cmake/sdl3.cmake)
# 	target_setup_sdl3_for_stylizer(api_tst)
# endif()

