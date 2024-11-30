spargel_add_library(base)
target_sources(base
  PRIVATE
    base.c
    platform.c
    allocator/allocator.cpp
    assert/panic.cpp
    assert/unreachable.cpp
    memory/placement_new.cpp
    string/string.cpp
    test/test.cpp
    test/test_manager.cpp
)

spargel_add_library(base_test_main)
target_sources(base_test_main
  PRIVATE
    test/test_main.cpp
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

spargel_add_test(vector_test)
target_sources(vector_test PRIVATE container/vector_test.cpp)
target_link_libraries(vector_test PRIVATE base)

spargel_add_test(coroutine_test)
target_sources(coroutine_test PRIVATE coroutine/coroutine_test.cpp)
target_link_libraries(coroutine_test PRIVATE base)

spargel_add_test(string_test)
target_sources(string_test PRIVATE string/string_test.cpp)
target_link_libraries(string_test PRIVATE base)

spargel_add_executable(panic_test)
target_sources(panic_test PRIVATE assert/panic_test.cpp)
target_link_libraries(panic_test PRIVATE base)

spargel_add_executable(unreachable_test)
target_sources(unreachable_test PRIVATE assert/unreachable_test.cpp)
target_link_libraries(unreachable_test PRIVATE base)
