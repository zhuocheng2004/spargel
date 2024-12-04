spargel_add_library(
    NAME base
    PRIVATE
        base.cpp
        output_stream.cpp
        panic.cpp
        platform.cpp
        allocator.cpp
        memory/placement_new.cpp
        string/string.cpp
        test/test.cpp
        test/test_manager.cpp
    PRIVATE_LINUX
        platform_linux.cpp
    PRIVATE_MACOS
        platform_mac.cpp
        backtrace_mac.cpp
    PRIVATE_WINDOWS
        platform_win.cpp
)

spargel_add_library(
    NAME base_test_main
    PRIVATE
        test/test_main.cpp
)

spargel_add_executable(
    NAME executable_path_demo
    PRIVATE executable_path_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME backtrace_demo
    PRIVATE backtrace_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME panic_demo
    PRIVATE panic_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME source_location_demo
    PRIVATE source_location_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME string_literal_demo
    PRIVATE string_literal_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME string_view_demo
    PRIVATE string_view_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME output_stream_demo
    PRIVATE output_stream_demo.cpp
    DEPS base
)

spargel_add_test(vector_test)
target_sources(vector_test PRIVATE vector_test.cpp)
target_link_libraries(vector_test PRIVATE base)

spargel_add_test(coroutine_test)
target_sources(coroutine_test PRIVATE coroutine_test.cpp)
target_link_libraries(coroutine_test PRIVATE base)

spargel_add_test(string_test)
target_sources(string_test PRIVATE string/string_test.cpp)
target_link_libraries(string_test PRIVATE base)
