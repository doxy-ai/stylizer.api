function(target_setup_glfw_for_stylizer TGT)
	target_link_libraries(${TGT} PUBLIC glfw)

	if (GLFW_BUILD_COCOA)
		target_compile_definitions(${TGT} PUBLIC _GLFW_COCOA)
		target_compile_options(${TGT} PUBLIC -x objective-c)
		target_link_libraries(${TGT} PUBLIC "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")
		target_compile_definitions(${TGT} PUBLIC STYLIZER_API_SURFACE_SUPPORT_COCOA)
	endif()
	if (GLFW_BUILD_WIN32)
		target_compile_definitions(${TGT} PUBLIC _GLFW_WIN32)
		target_compile_definitions(${TGT} PUBLIC STYLIZER_API_SURFACE_SUPPORT_WIN32)
	endif()
	if (GLFW_BUILD_X11)
		target_compile_definitions(${TGT} PUBLIC _GLFW_X11)
		target_compile_definitions(${TGT} PUBLIC STYLIZER_API_SURFACE_SUPPORT_X11)
	endif()
	if (GLFW_BUILD_WAYLAND)
		target_compile_definitions(${TGT} PUBLIC _GLFW_WAYLAND)
		target_compile_definitions(${TGT} PUBLIC STYLIZER_API_SURFACE_SUPPORT_WAYLAND)
	endif()
endfunction(target_setup_glfw_for_stylizer)