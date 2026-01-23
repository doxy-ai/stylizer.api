
add_subdirectory(stub)

set(STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS "webgpu" "none")
set(STYLIZER_GRAPHICS_CURRENT_BACKEND webgpu CACHE STRING "Backend that is currently set as the default for the Stylizer Graphics API")
set_property(CACHE STYLIZER_GRAPHICS_CURRENT_BACKEND PROPERTY STRINGS ${STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS})
string(TOLOWER ${STYLIZER_GRAPHICS_CURRENT_BACKEND} STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER)
if(NOT STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER IN_LIST STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS)
	message(FATAL_ERROR "The current backend must be one of ${STYLIZER_GRAPHICS_CURRENT_BACKEND_OPTIONS} not ${STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER}")
endif()

option(STYLIZER_GRAPHICS_ENABLE_WEBGPU "Should the Stylizer Graphics API support the WebGPU Backend?" ON)

add_library(stylizer_graphics_current_backend)

if(STYLIZER_GRAPHICS_ENABLE_WEBGPU)
	add_subdirectory(webgpu EXCLUDE_FROM_ALL)
	if(STYLIZER_GRAPHICS_CURRENT_BACKEND STREQUAL webgpu)
		target_compile_definitions(stylizer_graphics_current_backend PUBLIC -DSTYLIZER_CURRENT_BACKEND=1)
		target_link_libraries(stylizer_graphics_current_backend PUBLIC stylizer_graphics_webgpu)
	endif()
elseif(STYLIZER_GRAPHICS_CURRENT_BACKEND_LOWER STREQUAL webgpu)
	message(FATAL_ERROR "WebGPU set as the current backend but it is not enabled!")
endif(STYLIZER_GRAPHICS_ENABLE_WEBGPU)

target_sources(stylizer_graphics_current_backend PUBLIC
	FILE_SET stylizer_graphics_current_backend TYPE CXX_MODULES
	# BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		current_backend.cppm
)

add_library(stylizer::graphics::current_backend ALIAS stylizer_graphics_current_backend)