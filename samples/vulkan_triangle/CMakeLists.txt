add_executable(vulkan_triangle
  app.cc
  app_delegate.cc
  main.cc
  utils.cc
)
if (APPLE)
  target_sources(vulkan_triangle
    PRIVATE
      app_delegate_mac.mm
  )
endif ()
target_compile_features(vulkan_triangle PUBLIC cxx_std_20)
target_include_directories(vulkan_triangle
  PUBLIC
  "${PROJECT_SOURCE_DIR}"
  "${PROJECT_SOURCE_DIR}/third_party/vulkan_headers/repo/include"
)
target_link_libraries(vulkan_triangle PRIVATE base)
