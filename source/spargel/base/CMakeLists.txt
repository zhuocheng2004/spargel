spargel_add_library(
    NAME base
    PRIVATE
        allocator.cpp
        base.cpp
        platform.cpp
        string.cpp
        logging.cpp
    PRIVATE_ANDROID
        platform_linux.cpp
    PRIVATE_LINUX
        platform_linux.cpp
    PRIVATE_MACOS
        platform_mac.cpp
    PRIVATE_WINDOWS
        platform_win.cpp
    DEPS spargel_config
)

if (SPARGEL_IS_ANDROID OR SPARGEL_IS_LINUX)
    target_link_libraries(base PUBLIC m)
endif ()

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
    NAME log_demo
    PRIVATE log_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME tag_invoke_demo
    PRIVATE tag_invoke_demo.cpp
    DEPS base
)

spargel_add_executable(
    NAME vector_test
    PRIVATE vector_test.cpp
    DEPS base
)
