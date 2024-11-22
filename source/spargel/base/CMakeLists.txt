spargel_add_library(base)
target_sources(base
  PRIVATE
    base.c
)

if (SPARGEL_IS_LINUX)
  target_sources(base PRIVATE base_linux.c)
endif ()

if (SPARGEL_IS_WINDOWS)
  target_sources(base PRIVATE base_win.c)
endif ()

if (SPARGEL_IS_MACOS)
  target_sources(base PRIVATE base_mac.c)
endif ()

spargel_add_executable(demo_get_executable_path)
target_sources(demo_get_executable_path PRIVATE demo_get_executable_path.c)
target_link_libraries(demo_get_executable_path PRIVATE base)
