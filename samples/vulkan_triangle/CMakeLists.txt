spargel_add_executable(vulkan_triangle)
target_sources(vulkan_triangle
  PRIVATE
    app.cc
    app_delegate.cc
    main.cc
    utils.cc
    logging.cc
)
if (SPARGEL_IS_MACOS)
  target_sources(vulkan_triangle
    PRIVATE
      app_delegate_mac.mm
  )
endif ()
if (SPARGEL_IS_WINDOWS)
  target_sources(vulkan_triangle
    PRIVATE
      app_delegate_win.cc
  )
endif ()
target_link_libraries(vulkan_triangle PRIVATE Vulkan::Headers)

if (SPARGEL_IS_MACOS)
  find_library(APPKIT AppKit)
  find_library(QUARTZ_CORE QuartzCore)
  target_link_libraries(vulkan_triangle PRIVATE ${APPKIT} ${QUARTZ_CORE})
endif ()
