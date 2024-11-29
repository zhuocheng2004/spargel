spargel_add_library(base)
target_sources(base
  PRIVATE
    base.c
    platform.c
)

if (SPARGEL_IS_LINUX)
  target_sources(base PRIVATE platform_linux.c)
endif ()

if (SPARGEL_IS_WINDOWS)
  target_sources(base PRIVATE platform_win.c)
endif ()

if (SPARGEL_IS_MACOS)
  target_sources(base PRIVATE platform_mac.c)
endif ()

spargel_add_executable(demo_get_executable_path)
target_sources(demo_get_executable_path PRIVATE demo_get_executable_path.c)
target_link_libraries(demo_get_executable_path PRIVATE base)

spargel_add_executable(demo_backtrace)
target_sources(demo_backtrace PRIVATE demo_backtrace.c)
target_link_libraries(demo_backtrace PRIVATE base)
