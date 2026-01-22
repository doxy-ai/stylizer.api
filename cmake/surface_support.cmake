function (target_setup_surface_support_for_stylizer TGT)
	if(${STYLIZER_GRAPHICS_USE_COCOA}) # TODO: Is this the actual flag?
		target_compile_definitions(${TGT} PUBLIC STYLIZER_GRAPHICS_SURFACE_SUPPORT_COCOA)
	endif()
	# TODO: IOS Webkit Support
	if(${STYLIZER_GRAPHICS_USE_WINDOWS_UI}) # TODO: Is this the actual flag?
		target_compile_definitions(${TGT} PUBLIC STYLIZER_GRAPHICS_SURFACE_SUPPORT_WIN32)
	endif()
	if(${STYLIZER_GRAPHICS_USE_X11})
		target_compile_definitions(${TGT} PUBLIC STYLIZER_GRAPHICS_SURFACE_SUPPORT_X11)
	endif()
	if(${STYLIZER_GRAPHICS_USE_WAYLAND})
		target_compile_definitions(${TGT} PUBLIC STYLIZER_GRAPHICS_SURFACE_SUPPORT_WAYLAND)
	endif()
endfunction(target_setup_surface_support_for_stylizer)