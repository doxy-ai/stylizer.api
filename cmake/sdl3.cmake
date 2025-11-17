function(target_setup_sdl3_for_stylizer TGT)
	target_link_libraries(${TGT} PUBLIC SDL3::SDL3)

	if (APPLE)
		target_compile_options(${TGT} PRIVATE -x objective-c)
		target_link_libraries(${TGT} PRIVATE "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")

		if (IOS)
			target_link_libraries(${TGT} PRIVATE "-framework UIKit")
		else()
			target_link_libraries(${TGT} PRIVATE "-framework Cocoa")
		endif()
	endif()
endfunction(target_setup_sdl3_for_stylizer)