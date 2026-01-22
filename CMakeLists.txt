cmake_minimum_required(VERSION 3.30)
project(stylizer.graphics CXX)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(DEFINED ENV{XDG_SESSION_TYPE} AND "$ENV{XDG_SESSION_TYPE}" STREQUAL "wayland")
    set(STYLIZER_WAYLAND_PREFERRED ${LINUX})
else()
    set(STYLIZER_WAYLAND_PREFERRED FALSE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(STYLIZER_SPLIT_DWARF_PREFERRED ${PROJECT_IS_TOP_LEVEL})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(STYLIZER_SPLIT_DWARF_PREFERRED ${PROJECT_IS_TOP_LEVEL})
else()
	set(STYLIZER_SPLIT_DWARF_PREFERRED false)
endif()

set(STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS "webgpu" "none")
set(STYLIZER_GRAPHICS_CURRENT_BACKEND webgpu CACHE STRING "Backend that is currently set as the default for the Stylizer Graphics API")
set_property(CACHE STYLIZER_GRAPHICS_CURRENT_BACKEND PROPERTY STRINGS ${STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS})
string(TOLOWER ${STYLIZER_GRAPHICS_CURRENT_BACKEND} STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER)
if(NOT STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER IN_LIST STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS)
	message(FATAL_ERROR "The current backend must be one of ${STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS} not ${STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER}")
endif()

option(STYLIZER_GRAPHICS_BUILD_TEST "Should we build the test file?" ${PROJECT_IS_TOP_LEVEL})
option(STYLIZER_GRAPHICS_USE_SPLIT_DWARF "Should debugging information be included in separated files?" ${STYLIZER_SPLIT_DWARF_PREFERRED})

option(STYLIZER_GRAPHICS_USE_COCOA "Should we build cocoa support?" ${APPLE})
option(STYLIZER_GRAPHICS_USE_WINDOWS_UI "Should we build windows ui support?" ${WIN32})
option(STYLIZER_GRAPHICS_USE_X11 "Should we build X11 support?" ${LINUX})
option(STYLIZER_GRAPHICS_USE_WAYLAND "Should we build wayland support?" ${STYLIZER_WAYLAND_PREFERRED})

# option(STYLIZER_GRAPHICS_ENABLE_WEBGPU "Should the Stylizer Graphics API support the WebGPU Backend?" ON)

set(CMAKE_CXX_STANDARD 26)

if(STYLIZER_GRAPHICS_USE_SPLIT_DWARF)
	add_compile_options(-gsplit-dwarf)
endif()

add_subdirectory(extern/slcross EXCLUDE_FROM_ALL)
add_subdirectory(extern/stdmath EXCLUDE_FROM_ALL)

add_library(stylizer_graphics)
target_sources(stylizer_graphics PUBLIC
	FILE_SET stylizer_graphics TYPE CXX_MODULES
	# BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	FILES 
		api.cppm extern/slcross/slcross.cppm
		backends/stub/stub.cppm
		util/errors.cppm
		util/auto_release.cppm 
		util/convertible_to_pointer.cppm 
		util/flags.cppm extern/slcross/thirdparty/magic_enum/module/magic_enum.cppm
		util/spans.cppm 
		util/string2magic.cppm
)
target_link_libraries(stylizer_graphics PUBLIC slcross magic_enum std::math)
target_include_directories(stylizer_graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/backends)

# add_library(stylizer::graphics alias stylizer_graphics)

# if(STYLIZER_GRAPHICS_ENABLE_WEBGPU)
# 	add_subdirectory(backends/webgpu EXCLUDE_FROM_ALL)
# elseif(STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER STREQUAL webgpu)
# 	message(FATAL_ERROR "WebGPU set as the current backend but it is not enabled!")
# endif(STYLIZER_GRAPHICS_ENABLE_WEBGPU)

# if(${STYLIZER_GRAPHICS_BUILD_TEST})
# 	find_package(glfw3 CONFIG REQUIRED)

# 	add_executable(api_tst "test.cpp")
# 	target_link_libraries(api_tst PUBLIC stylizer_graphics glfw)

# 	include(cmake/glfw.cmake)
# 	target_setup_glfw_for_stylizer(api_tst)
# endif()
