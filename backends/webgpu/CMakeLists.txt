add_library(stylizer_api_webgpu device.cpp)
target_link_libraries(stylizer_api_webgpu PUBLIC stylizer_api)
if(NOT EMSCRIPTEN)
    include(../../cmake/surface_support.cmake)
    target_setup_surface_support_for_stylizer(stylizer_api_webgpu)

    option(DAWN_USE_COCOA "Should we build cocoa support?" ${STYLIZER_USE_COCOA})
    option(DAWN_USE_WINDOWS_UI "Should we build windows ui support?" ${STYLIZER_USE_WINDOWS_UI})
    option(DAWN_USE_X11 "Should we build X11 support?" ${STYLIZER_USE_X11})
    option(DAWN_USE_WAYLAND "Should we build wayland support?" ${STYLIZER_USE_WAYLAND})

    include(cmake/FetchDawn.cmake)
    target_link_libraries(stylizer_api_webgpu PUBLIC webgpu_dawn)
endif()

add_library(stylizer::api::webgpu ALIAS stylizer_api_webgpu)