spargel_add_executable(
    NAME vulkan_triangle
    PRIVATE
        app.cc
        app_delegate.cc
        main.cc
        utils.cc
        logging.cc
    DEPS ui
)
if (SPARGEL_IS_MACOS)
    target_sources(vulkan_triangle
        PRIVATE
            app_delegate_mac.mm
    )
endif ()
if (SPARGEL_IS_WINDOWS)
#  target_sources(vulkan_triangle
#    PRIVATE
#      app_delegate_win.cc
#  )
endif ()
target_link_libraries(vulkan_triangle PRIVATE ui Vulkan::Headers)
target_include_directories(vulkan_triangle PRIVATE "${PROJECT_SOURCE_DIR}/games")
