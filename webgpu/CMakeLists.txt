
add_library(stylizer_api_webgpu device.cpp render_pass.cpp texture_view.cpp texture.cpp)
target_link_libraries(stylizer_api_webgpu PUBLIC magic_enum slcross::slcross webgpu_dawn)
if(EMSCRIPTEN)
	target_include_directories(stylizer_api_webgpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/emscripten)
else()
	target_include_directories(stylizer_api_webgpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dawn)
endif()

add_library(stylizer::api::webgpu ALIAS stylizer_api_webgpu)

if(STYLIZER_API_CURRENT_BACKEND_LOWER STREQUAL webgpu)
	add_library(stylizer::api ALIAS stylizer_api_webgpu)
endif()