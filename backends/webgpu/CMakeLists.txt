add_library(stylizer_graphics_webgpu_impl)
target_sources(stylizer_graphics_webgpu_impl PUBLIC
	FILE_SET stylizer_graphics_webgpu_impl TYPE CXX_MODULES
	# BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	FILES 
        common.cppm
		bind_group.cppm 
		buffer.cppm 
		command_buffer.cppm 
		command_encoder_base.cppm 
		command_encoder.cppm 
		compute_pipeline.cppm 
		device.cppm 
		render_pass.cppm 
		render_pipeline.cppm 
		shader.cppm 
		surface.cppm 
		texture_view.cppm 
		texture.cppm
		# webgpu.cppm
)
target_link_libraries(stylizer_graphics_webgpu_impl PUBLIC stylizer_graphics)
target_compile_definitions(stylizer_graphics_webgpu_impl PUBLIC -DSTYLIZER_GRAPHICS_WEBGPU_AVAILABLE)
if(NOT EMSCRIPTEN)
    include(../../cmake/surface_support.cmake)
    target_setup_surface_support_for_stylizer(stylizer_graphics_webgpu_impl)

    option(DAWN_USE_COCOA "Should we build cocoa support?" ${STYLIZER_GRAPHICS_USE_COCOA})
    option(DAWN_USE_WINDOWS_UI "Should we build windows ui support?" ${STYLIZER_GRAPHICS_USE_WINDOWS_UI})
    option(DAWN_USE_X11 "Should we build X11 support?" ${STYLIZER_GRAPHICS_USE_X11})
    option(DAWN_USE_WAYLAND "Should we build wayland support?" ${STYLIZER_GRAPHICS_USE_WAYLAND})

    include(cmake/FetchDawn.cmake)
    target_link_libraries(stylizer_graphics_webgpu_impl PUBLIC webgpu_dawn)
endif()

add_library(stylizer_graphics_webgpu bind_group.cpp buffer.cpp command_buffer.cpp compute_pipeline.cpp 
	device.cpp device.create.cpp render_pass.cpp render_pipeline.cpp shader.cpp surface.cpp texture.cpp
)
target_sources(stylizer_graphics_webgpu PUBLIC
	FILE_SET stylizer_graphics_webgpu TYPE CXX_MODULES
	# BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	FILES 
        # common.cppm
		# bind_group.cppm 
		# buffer.cppm 
		# command_buffer.cppm 
		# command_encoder.cppm 
		# compute_pipeline.cppm 
		# device.cppm 
		# render_pass.cppm 
		# render_pipeline.cppm 
		# shader.cppm 
		# surface.cppm 
		# texture_view.cppm 
		# texture.cppm
		webgpu.cppm
)
target_link_libraries(stylizer_graphics_webgpu PUBLIC stylizer_graphics_webgpu_impl)

add_library(stylizer::graphics::webgpu ALIAS stylizer_graphics_webgpu)