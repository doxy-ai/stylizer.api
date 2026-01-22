
add_library(stylizer_graphics_stub_impl)
target_sources(stylizer_graphics_stub_impl PUBLIC
	FILE_SET stylizer_graphics_stub_impl TYPE CXX_MODULES
	# BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	FILES 
		bind_group.cppm 
		buffer.cppm 
		command_buffer.cppm 
		command_encoder.cppm 
		compute_pipeline.cppm 
		device.cppm 
		render_pass.cppm 
		render_pipeline.cppm 
		shader.cppm 
		surface.cppm 
		texture_view.cppm 
		texture.cppm
)
target_link_libraries(stylizer_graphics_stub_impl PUBLIC stylizer_graphics)


add_library(stylizer_graphics_stub)
target_sources(stylizer_graphics_stub PUBLIC FILE_SET stylizer_graphics_stub TYPE CXX_MODULES FILES stub.cppm)
target_link_libraries(stylizer_graphics_stub PUBLIC stylizer_graphics_stub_impl)

add_library(stylizer::graphics::stub ALIAS stylizer_graphics_stub)