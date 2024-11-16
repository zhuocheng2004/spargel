add_executable(vulkan_triangle
  app.cc
  app_delegate.cc
  main.cc
  utils.cc
  logging.cc
)
if (APPLE)
  target_sources(vulkan_triangle
    PRIVATE
      app_delegate_mac.mm
  )
endif ()
if (WIN32)
  target_sources(vulkan_triangle
    PRIVATE
      app_delegate_win.cc
  )
endif ()
target_compile_features(vulkan_triangle PUBLIC cxx_std_20)
target_include_directories(vulkan_triangle
  PUBLIC
  "${PROJECT_SOURCE_DIR}"
)
target_link_libraries(vulkan_triangle PRIVATE Vulkan::Headers)

if (APPLE)
  find_library(APPKIT AppKit)
  find_library(QUARTZ_CORE QuartzCore)
  target_link_libraries(vulkan_triangle PRIVATE ${APPKIT} ${QUARTZ_CORE})
endif ()
